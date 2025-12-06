// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "Building.h"
#include "FactionController.h"
#include "Faction.generated.h"

USTRUCT(BlueprintType)
struct FWorkerStats
{
	GENERATED_USTRUCT_BODY()

public:
	int working;
	int available;

	int workingEnergyCost;
	int workingFoodCost;
	int workingProductionCost;
};

USTRUCT(BlueprintType)
struct FInventoryStat
{
	GENERATED_USTRUCT_BODY()

public:
	int currentResources = 0;
	int maxResources = 10;
	int resourcePerTick = 0;
	int lossesPerDay = 0;
};

USTRUCT(BlueprintType)
struct FRelationshipStats
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite) Factions faction = Factions::None;

	//Enum relationship with target faction, default is neutral
	UPROPERTY(BlueprintReadWrite) FactionRelationship relationship = FactionRelationship::Neutral;

	//Scale from 0 to 1 in terms of a faction's hostility toward the target
	//0 turns this faction into an ally, 1 turns it into an enemy
	UPROPERTY(BlueprintReadWrite) float hostilityScale = 0.5f;
};

UCLASS(BlueprintType)
class CAPSTONEPROJECT_API UFaction : public UObject
{
	GENERATED_BODY()

public:
	UFaction();

	UPROPERTY(BlueprintReadWrite) TMap<EStratResources, FInventoryStat> resourceInventory;
	UPROPERTY() TMap<WorkerType, FWorkerStats> availableWorkers;
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
	TMap<FString, TArray<int32>> armyNamesHuman;
	TMap<FString, TArray<int32>> armyNamesAlien;
	UPROPERTY() int maxNameShare = 3;

	FactionRelationship GetFactionRelationship(Factions targetFaction);
	FactionRelationship GetFactionRelationship(AActor* target);

	UFUNCTION() void IncreaseHostility(Factions targetFaction, float amount);
	UFUNCTION() void LowerHostility(Factions targetFaction, float amount);

	UFUNCTION() void SetFaction(Factions newFaction);
	UFUNCTION() Factions GetFaction();
	UFUNCTION() FString GetFactionName() const;
	UFUNCTION() bool IsAIControlled();

	UFUNCTION() void UpdateResourceCosts();
	UFUNCTION() FResourceGainLoss GetResourceRates();
	UFUNCTION() void FeedPop();
	UFUNCTION() void ConsumeEnergy();

	UFUNCTION() void BuildRandomBuilding();
	UFUNCTION() TArray<ABaseHex*> GetHexesOfResource(EStratResources resource, int minValue = 2, bool includeHexesWithBuildings = false);
	UFUNCTION() TArray<AOutpost*> GetFactionOutposts();

	UFUNCTION() void SpawnEnemy();
	UFUNCTION() int GetArmyTroopCount();

	UFUNCTION() void SetDaysToTroopBuild(unsigned int days);
	UFUNCTION() void SetDaysToBuildingBuild(unsigned int days);

	UFUNCTION(BlueprintCallable, BlueprintPure) const TArray<FRelationshipStats> GetFactionRelationships() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) TMap<EStratResources, int> GetAvailableResources() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) int GetResourceMax() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) AFactionController* GetFactionController() const;
	UFUNCTION() void SetFactionController(AFactionController* setController);

	UFUNCTION(BlueprintCallable) void CollectResource(EStratResources resource, int amount);
	
private:

	UPROPERTY() Factions faction;
	UPROPERTY() FString factionName;
	UPROPERTY() AFactionController* controller;

	UPROPERTY() TMap<Factions, FRelationshipStats> factionRelationships;
	UFUNCTION() void SetFactionRelationship(Factions targetFaction, FactionRelationship newRelationship);
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
