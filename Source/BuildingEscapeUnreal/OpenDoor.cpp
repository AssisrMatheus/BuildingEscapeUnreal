// Fill out your copyright notice in the Description page of Project Settings.

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

	TargetYaw = CurrentRotation.Yaw + TargetYaw;
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FRotator CurrentRotation = GetOwner()->GetActorRotation();

	if ((CurrentRotation.Yaw - TargetYaw) < 90)
	{
		CurrentRotation.Yaw = FMath::Lerp(CurrentRotation.Yaw, TargetYaw, 0.05f);
	}

	GetOwner()->SetActorRotation(CurrentRotation);
}
