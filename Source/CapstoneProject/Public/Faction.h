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
		int lossesPerDay = 0;
	};

	Factions faction;
	UPROPERTY(BlueprintReadWrite) TMap<StratResources, InventoryStat> resourceInventory;
	UPROPERTY() TMap<WorkerType, WorkerStats> availableWorkers;
	UPROPERTY() TArray<ATroop*> allUnits;
	UPROPERTY()TArray<ABuilding*> allBuildings;
	UPROPERTY() TArray<ABaseHex*> ownedHexes;

	UPROPERTY() TArray<AActor*> targetList;

	UPROPERTY() TMap<Factions, FactionRelationship> factionRelationships;

	void FindActiveFactions();

	UPROPERTY() int daysTillStarve = 3;
	UPROPERTY() int currStarveDays = 0;
	UPROPERTY() bool starving = false;

	UPROPERTY() int daysTillPowerOutage = 3;
	UPROPERTY() int currPowerDays = 0;
	UPROPERTY() bool powerOutage = false;

	UPROPERTY() UMaterialInterface* factionColor;
};
