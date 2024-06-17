// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "Building.h"

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

	struct FRelationshipStats
	{
		//Enum relationship with target faction, default is neutral
		FactionRelationship relationship = FactionRelationship::Neutral;

		//Scale from 0 to 1 in terms of a faction's hostility toward the target
		//0 turns this faction into an ally, 1 turns it into an enemy
		float hostilityScale = 0.5f;
	};

	UPROPERTY(BlueprintReadWrite) TMap<StratResources, InventoryStat> resourceInventory;
	UPROPERTY() TMap<WorkerType, WorkerStats> availableWorkers;
	UPROPERTY() TSet<ATroop*> allUnits = TSet<ATroop*>();
	UPROPERTY() TSet<ABuilding*> allBuildings = TSet<ABuilding*>();
	UPROPERTY() TSet<ABaseHex*> ownedHexes = TSet<ABaseHex*>();
	UPROPERTY() TMap<ABaseHex*, Factions> targetList = TMap<ABaseHex*, Factions>();

	void FindActiveFactions();

	UPROPERTY() int daysTillStarve = 3;
	UPROPERTY() int currStarveDays = 0;
	UPROPERTY() bool starving = false;

	UPROPERTY() int daysTillPowerOutage = 3;
	UPROPERTY() int currPowerDays = 0;
	UPROPERTY() bool powerOutage = false;

	UPROPERTY() UMaterialInterface* factionColor;
	UPROPERTY() TMap<FString, TArray<int32>> armyNamesHuman;
	UPROPERTY() TMap<FString, TArray<int32>> armyNamesAlien;
	UPROPERTY() int maxNameShare = 3;

	UFUNCTION() FactionRelationship GetFactionRelationship(Factions targetFaction);
	UFUNCTION() FactionRelationship GetFactionRelationship(AActor* target);

	UFUNCTION() void SetFactionRelationship(Factions targetFaction, FactionRelationship newRelationship);
	UFUNCTION() void IncreaseHostility(Factions targetFaction, float& amount);
	UFUNCTION() void LowerHostility(Factions targetFaction, float& amount);

	UFUNCTION() void SetFaction(Factions newFaction);
	UFUNCTION() bool IsAIControlled();

private:

	UPROPERTY() Factions faction;
	UPROPERTY() bool AIControlled = false;

	UPROPERTY() TMap<Factions, FRelationshipStats> factionRelationships;
	UFUNCTION() void CleanTargetPool();
	UFUNCTION() void GetTargetsOfAllies();
};
