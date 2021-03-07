// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		// UE_LOG(LogTemp, Warning, TEXT("%s found InputComponent"), *GetOwner()->GetName())
		InputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no instance of UInputComponent"), *GetOwner()->GetName())
	}
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no instance of UPhysicsHandleComponent"), *GetOwner()->GetName())
	}
}

FVector UGrabber::GetReachLocation(FVector PlayerViewPointLocation, FRotator PlayerViewPointRotation) const
{
	return PlayerViewPointLocation + (PlayerViewPointRotation.Vector() * Reach);
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach(FVector ReachLocation, FVector PlayerViewPointLocation) const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
			OUT Hit,
			PlayerViewPointLocation,
			ReachLocation,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			TraceParams);

	return Hit;
}

void UGrabber::Grab()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	FVector ReachLocation = GetReachLocation(PlayerViewPointLocation, PlayerViewPointRotation);
	FHitResult HitResult = GetFirstPhysicsBodyInReach(ReachLocation, PlayerViewPointLocation);
	AActor *ActorHit = HitResult.GetActor();

	if (ActorHit && PhysicsHandle)
	{
		UPrimitiveComponent *ComponentToGrab = HitResult.GetComponent();
		PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, ReachLocation);
	}
}

void UGrabber::Release()
{
	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->ReleaseComponent();
	}
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	SetupInputComponent();
	FindPhysicsHandle();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

		PhysicsHandle->SetTargetLocation(GetReachLocation(PlayerViewPointLocation, PlayerViewPointRotation));
	}
}
