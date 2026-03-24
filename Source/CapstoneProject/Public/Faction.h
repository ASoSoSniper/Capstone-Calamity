// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "Building.h"
#include "FactionController.h"
#include "Faction.generated.h"

#pragma region Structs
USTRUCT(BlueprintType)
struct FWorkerStats
{
	GENERATED_USTRUCT_BODY()

public:
	int working;
	int available;
	int maxAcquired;

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

USTRUCT(BlueprintType)
struct FBuildingSet
{
	GENERATED_USTRUCT_BODY()

public:
	FBuildingStats buildingStats;
	TSet<ABuilding*> buildings;
};

USTRUCT(BlueprintType)
struct FHexSet
{
	GENERATED_USTRUCT_BODY()

public:
	FHexInfo hexInfo;
	TSet<ABaseHex*> hexes;
};
#pragma endregion

class UUAI_PriorityManager_Hex;

UCLASS(BlueprintType)
class CAPSTONEPROJECT_API UFaction : public UObject
{
	GENERATED_BODY()

#pragma region General Logic
public:
	UFaction();

	UFUNCTION() void SetFaction(Factions newFaction);
	UFUNCTION() Factions GetFaction();
	UFUNCTION() FString GetFactionName() const;
	UFUNCTION() bool IsAIControlled();

	UFUNCTION(BlueprintCallable, BlueprintPure) AFactionController* GetFactionController() const;
	UFUNCTION() void SetFactionController(AFactionController* setController);

	void FindActiveFactions();

	UPROPERTY() UMaterialInterface* factionColor;
	TMap<FString, TArray<int32>> armyNamesHuman;
	TMap<FString, TArray<int32>> armyNamesAlien;
	UPROPERTY() int maxNameShare = 3;

	UPROPERTY() TSet<ATroop*> allUnits = TSet<ATroop*>();
private:
	UPROPERTY() Factions faction;
	UPROPERTY() FString factionName;
	UPROPERTY() AFactionController* controller;
#pragma endregion
#pragma region Resources
public:
	UFUNCTION(BlueprintCallable, BlueprintPure) TMap<EStratResources, int> GetAvailableResources() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) int GetResourceMax() const;
	
	UFUNCTION(BlueprintCallable) void CollectResource(EStratResources resource, int amount);
	UFUNCTION(BlueprintCallable) void ConsumeResource(EStratResources resource, int amount);
	UFUNCTION(BlueprintCallable) void SetResources(const TMap<EStratResources, int>& resources);

	UPROPERTY(BlueprintReadWrite) TMap<EStratResources, FInventoryStat> resourceInventory;
private:	
#pragma endregion
#pragma region Resource Costs
public:
	void SetFoodAndDeathCosts(int foodPerNonWorkersVar, int foodPerWorkersVar, int popDeathsPerFoodMissingVar, int popDeathsPerPowerMissingVar);
	UFUNCTION() void UpdateResourceCosts();
	UFUNCTION() FResourceGainLoss GetResourceRates();

	UFUNCTION() void FeedPop();
	UFUNCTION() void ConsumeEnergy();

	UFUNCTION(BlueprintCallable, BlueprintPure) TMap<EStratResources, int> GetNetResourcesPerDay(bool includeIncompleteBuildings) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) int GetResourceGainsPerDay(EStratResources resource, bool includeIncompleteBuildings) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) int GetResourceLossesPerDay(EStratResources resource) const;

	UPROPERTY() int daysTillStarve = 3;
	UPROPERTY() int currStarveDays = 0;
	UPROPERTY() bool starving = false;

	UPROPERTY() int daysTillPowerOutage = 3;
	UPROPERTY() int currPowerDays = 0;
	UPROPERTY() bool powerOutage = false;
private:
	UFUNCTION() void CalculateFoodCost(int& availableWorkerCost, int& workingWorkerCost);
	UFUNCTION() int CalculateEnergyCost();

	UFUNCTION() void StarvePop(int foodCost);
	UFUNCTION() void PowerOutage(int energyCost);
	
	int foodPerNonWorkers;
	int foodPerWorkers;
	int popDeathsPerFoodMissing;
	int popDeathsPerPowerMissing;
#pragma endregion
#pragma region Faction Relationships
public:
	UFUNCTION(BlueprintCallable, BlueprintPure) const TArray<FRelationshipStats> GetFactionRelationships() const;
	FactionRelationship GetFactionRelationship(Factions targetFaction);
	FactionRelationship GetFactionRelationship(AActor* target);

	UFUNCTION() void IncreaseHostility(Factions targetFaction, float amount);
	UFUNCTION() void LowerHostility(Factions targetFaction, float amount);
private:
	UFUNCTION() void SetFactionRelationship(Factions targetFaction, FactionRelationship newRelationship);
	UPROPERTY() TMap<Factions, FRelationshipStats> factionRelationships;
#pragma endregion
#pragma region Hexes
public:
	UFUNCTION(BlueprintCallable, BlueprintPure) bool OwnsHex(ABaseHex* hex) const;
	UFUNCTION(BlueprintCallable) void ClaimHex(ABaseHex* hex);
	UFUNCTION(BlueprintCallable) void DropHex(ABaseHex* hex);
	UFUNCTION(BlueprintCallable, BlueprintPure) const TMap<TerrainType, FHexSet>& GetOwnedHexes() const;

	UFUNCTION(BlueprintCallable, BlueprintPure) int GetOccupiedHexCount() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) int GetOwnedHexCount(bool includeNonBuildable = false) const;
private:
	UPROPERTY() TMap<TerrainType, FHexSet> ownedHexes = TMap<TerrainType, FHexSet>();
#pragma endregion
#pragma region Buildings
public:
	UFUNCTION() const TSet<ABuilding*>& GetBuildingsOfType(SpawnableBuildings buildingType) const;
	void AddBuildingToFaction(ABuilding* building);
	void RemoveBuildingFromFaction(ABuilding* building);
private:
	UPROPERTY() TMap<SpawnableBuildings, FBuildingSet> allBuildings;
#pragma endregion
#pragma region Workers
public:
	UFUNCTION(BlueprintCallable) int SetWorkerAvailability(bool toWorking, WorkerType workerType, int desiredWorkers);
	UFUNCTION(BlueprintCallable) bool ConsumeWorkers(WorkerType workerType, int count);

	UFUNCTION(BlueprintCallable, BlueprintPure) float GetPopAlpha() const;

	UPROPERTY() TMap<WorkerType, FWorkerStats> availableWorkers;
private:
	UFUNCTION() void RemoveWorkers(WorkerType workerType);
	UFUNCTION() void KillPopulation(int amount);
#pragma endregion
#pragma region Enemy Targeting
public:
	UPROPERTY() TMap<ABaseHex*, Factions> targetList = TMap<ABaseHex*, Factions>();
private:
	UFUNCTION() void CleanTargetPool();
	UFUNCTION() void GetTargetsOfAllies();
	UFUNCTION() void TargetBuildingsOfFaction(Factions targetFaction);
#pragma endregion
#pragma region Priority Targeting
public:
	UFUNCTION(BlueprintCallable, BlueprintPure) ABaseHex* GetPriorityHex_Workers(EStratResources resource) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ABaseHex* GetPriorityHex_Building(SpawnableBuildings building) const;
#pragma endregion;
};
