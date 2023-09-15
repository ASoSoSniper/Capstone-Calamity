// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementAI.h"
#include "Building.h"

/**
 * 
 */
class CAPSTONEPROJECT_API Faction
{
public:
	Faction();
	~Faction();

	Factions faction;

	TMap<StratResources, int> resourceInventory;
	TArray<AMovementAI*> allUnits;
	TArray<ABuilding*> allBuildings;
};
