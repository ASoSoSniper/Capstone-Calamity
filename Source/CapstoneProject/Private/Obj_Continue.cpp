// Fill out your copyright notice in the Description page of Project Settings.


#include "Obj_Continue.h"
#include "CapstoneProjectGameModeBase.h"

void UObj_Continue::SetupObjective(UFaction* faction)
{
	Super::SetupObjective(faction);

	if (ticksToComplete <= 0)
		CompleteObjective();
	else
		ACapstoneProjectGameModeBase::onDateTick.AddDynamic(this, &UObj_Continue::HandleWorldTick);
}

void UObj_Continue::CleanupObjective()
{
	Super::CleanupObjective();

	currentTicks = 0;
	ACapstoneProjectGameModeBase::onDateTick.RemoveDynamic(this, &UObj_Continue::HandleWorldTick);
}

void UObj_Continue::HandleWorldTick(const FDateTickUpdate& update)
{
	currentTicks++;
	if (currentTicks >= ticksToComplete) CompleteObjective();
}
