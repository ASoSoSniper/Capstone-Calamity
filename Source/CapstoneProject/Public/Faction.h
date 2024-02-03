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

		int workingEnergyCost;
		int workingFoodCost;
		int workingProductionCost;
	};

	struct InventoryStat
	{
		int currentResources = 0;
		int maxResources = 10;
		int resourcePerTick = 0;
	};

	Factions faction;
	UPROPERTY(BlueprintReadWrite) TMap<StratResources, InventoryStat> resourceInventory;
	TMap<WorkerType, WorkerStats> availableWorkers;
	TArray<ATroop*> allUnits;
	TArray<ABuilding*> allBuildings;
	TArray<ABaseHex*> ownedHexes;

	TMap<Factions, FactionRelationship> factionRelationships;

	void FindActiveFactions();

	int daysTillStarve = 1;
	int currStarveDays = 0;
	bool starving = false;
};
