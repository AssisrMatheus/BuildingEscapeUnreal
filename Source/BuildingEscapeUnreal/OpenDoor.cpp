// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "OpenDoor.h"
#include "GameFramework/Actor.h"

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

	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && ActorThatOpens)
	{
		if (PressurePlate->IsOverlappingActor(ActorThatOpens))
		{
			OpenDoor(DeltaTime);
			DoorLastOpened = GetWorld()->GetTimeSeconds();
		}
		else if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
	else
	{
		FString ObjectName = GetOwner()->GetName();
		UE_LOG(LogTemp, Error, TEXT("%s has OpenDoor component but no PressurePlate && ActorThatOpens"), *ObjectName);
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	if ((CurrentRotation.Yaw - OpenYaw) < 90)
	{
		CurrentRotation.Yaw = FMath::Lerp(CurrentRotation.Yaw, OpenYaw, DoorOpenSpeed * DeltaTime);
		GetOwner()->SetActorRotation(CurrentRotation);
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	if (CurrentRotation.Yaw > CloseYaw)
	{
		CurrentRotation.Yaw = FMath::Lerp(CurrentRotation.Yaw, CloseYaw, DoorCloseSpeed * DeltaTime);
		GetOwner()->SetActorRotation(CurrentRotation);
	}
}
