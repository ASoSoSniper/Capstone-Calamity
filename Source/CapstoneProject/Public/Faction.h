// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "Building.h"

enum AIBehaviorStates
{
	AIInactive,
	Expansion,
	War
};

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
	void SetFoodAndDeathCosts(int foodPerNonWorkersVar, int foodPerWorkersVar, int popDeathsPerFoodMissingVar, int popDeathsPerPowerMissingVar);

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

	UFUNCTION() void UpdateResourceCosts();
	UFUNCTION() void FeedPop();
	UFUNCTION() void ConsumeEnergy();

	UFUNCTION() void BuildRandomBuilding();

	UFUNCTION() void SpawnEnemy();
	UFUNCTION() int GetArmyTroopCount();

	UFUNCTION() void SetDaysToTroopBuild(unsigned int days);
	UFUNCTION() void SetDaysToBuildingBuild(unsigned int days);
	
private:

	UPROPERTY() Factions faction;
	UPROPERTY() AIBehaviorStates behaviorState = AIBehaviorStates::AIInactive;
	UFUNCTION() void DetermineBehaviorState();


	UPROPERTY() TMap<Factions, FRelationshipStats> factionRelationships;
	UFUNCTION() void CleanTargetPool();
	UFUNCTION() void GetTargetsOfAllies();
	UFUNCTION() void TargetBuildingsOfFaction(Factions targetFaction);

	UFUNCTION() void CalculateFoodCost(int& availableWorkerCost, int& workingWorkerCost);
	UFUNCTION() int CalculateEnergyCost();

	UFUNCTION() void StarvePop(int foodCost);
	UFUNCTION() void PowerOutage(int energyCost);
	UFUNCTION() void RemoveWorkers(WorkerType workerType);
	UFUNCTION() void KillPopulation(int cost, int deathsPerResource);

	int foodPerNonWorkers;
	int foodPerWorkers;
	int popDeathsPerFoodMissing;
	int popDeathsPerPowerMissing;

	int currentDaysTillArmySpawn = 0;
	int currentDaysTillArmyGrowth = 0;
	int currentDaysTillBuildingSpawn = 0;

	int troopsInArmy = 0;
	int maxTroopsInArmy = 20;

	int daysTillArmySpawn = 1;
	int daysTillArmyGrowth = 10;
	int daysTillBuildingSpawn = 2;
};
