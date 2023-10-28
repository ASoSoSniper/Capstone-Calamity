// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "Building.h"

/**
 * 
 */
class CAPSTONEPROJECT_API Faction
{
public:
	Faction();
	~Faction();

	struct WorkerStats
	{
		int working;
		int available;
	};

	Factions faction;

	TMap<StratResources, int> resourceInventory;
	TMap<WorkerType, WorkerStats> availableWorkers;
	TArray<ATroop*> allUnits;
	TArray<ABuilding*> allBuildings;

	TMap<Factions, FactionRelationship> factionRelationships;

	void FindActiveFactions();
};
