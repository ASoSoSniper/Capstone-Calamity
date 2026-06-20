// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_PriorityManager_Troops.h"
#include "Faction.h"
#include "UAI_HexCondition.h"
#include "UAI_TroopCondition.h"

// Sets default values for this component's properties
UUAI_PriorityManager_Troops::UUAI_PriorityManager_Troops()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUAI_PriorityManager_Troops::Initialize(UFaction* faction)
{
	if (parentFaction) return;

	parentFaction = faction;
}

