// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicPosition.h"

// Sets default values for this component's properties
UCinematicPosition::UCinematicPosition()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	positionOffset = FVector(360.f, 0.f, 317.f);
}


// Called when the game starts
void UCinematicPosition::BeginPlay()
{
	Super::BeginPlay();

	//targetPosition = GetOwner()->GetActorLocation() + positionOffset;
}


// Called every frame
void UCinematicPosition::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

