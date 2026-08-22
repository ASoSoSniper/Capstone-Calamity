// Fill out your copyright notice in the Description page of Project Settings.


#include "PathingConditionDisplay.h"
#include "BaseHex.h"
#include "Troop.h"

APathingConditionDisplay::APathingConditionDisplay()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APathingConditionDisplay::Initialize_Implementation(const TArray<FString>& conditions)
{
}

