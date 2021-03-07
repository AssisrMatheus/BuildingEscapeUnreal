// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	OpenYaw = CurrentRotation.Yaw + OpenYaw;
	CloseYaw = CurrentRotation.Yaw;

	FindAudioComponent();
	FindPressurePlate();
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing audio component"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindPressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s is missing the pressure plate"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() >= MassToOpenDoor)
	{
		OpenDoor(DeltaTime);
	}
	else if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
	{
		CloseDoor(DeltaTime);
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	HasCloseDoorSoundPlayed = false;

	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	if ((CurrentRotation.Yaw - OpenYaw) < 90)
	{
		CurrentRotation.Yaw = FMath::Lerp(CurrentRotation.Yaw, OpenYaw, DoorOpenSpeed * DeltaTime);
		GetOwner()->SetActorRotation(CurrentRotation);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}

	if (AudioComponent && !HasOpenDoorSoundPlayed)
	{
		AudioComponent->Play();
		HasOpenDoorSoundPlayed = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	HasOpenDoorSoundPlayed = false;

	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	if (CurrentRotation.Yaw > CloseYaw)
	{
		CurrentRotation.Yaw = FMath::Lerp(CurrentRotation.Yaw, CloseYaw, DoorCloseSpeed * DeltaTime);
		GetOwner()->SetActorRotation(CurrentRotation);
	}

	if (AudioComponent && !HasCloseDoorSoundPlayed)
	{
		AudioComponent->Play();
		HasCloseDoorSoundPlayed = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	if (PressurePlate)
	{
		TArray<AActor *> OverlappingActors;
		PressurePlate->GetOverlappingActors(OverlappingActors);

		for (AActor *Actor : OverlappingActors)
		{
			TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}

	return TotalMass;
}