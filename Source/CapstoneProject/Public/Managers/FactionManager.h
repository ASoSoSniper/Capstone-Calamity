// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitActions.h"
#include "WorkerStats.h"
#include "Faction.h"
#include "FactionController.h"
#include "StratResources.h"
#include "FactionManager.generated.h"

UCLASS(HideCategories = (Rendering, Replication, Input, Actor, LOD, HLOD, Collision, Physics, Cooking, Events, WorldPartition, Events, DataLayers))
class CAPSTONEPROJECT_API AFactionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFactionManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static bool Initialized();

	static UFaction* GetFactionObject(EFactions faction);
	static const TMap<EFactions, UFaction*>& GetAllFactions();
	static UMaterialInterface* GetFactionColor(EFactions faction);
	static bool FactionAIEnabled();

	static int32 GetStartingResourceCap();
	static int32 GetTicksTillHarvest();
	static int32 GetPopDeathsPerFoodMissing();
	static int32 GetPopDeathsPerPowerMissing();
	static int32 GetDaysTillStarvation();
	static int32 GetDaysTillPowerOutage();

private:
	EFactions CreateNewFaction();
	UFUNCTION() void HandleDateTick(const FDateTickUpdate& date);

	static AFactionManager* _instance;
	TMap<EFactions, UFaction*> _activeFactions;
	TMap<EFactions, UMaterialInterface*> _factionColors;
	int32 _factionCount = 0;
	int32 _currTicksTillHarvest = 0;

	UPROPERTY(EditAnywhere, Category = "Faction Properties") bool enableFactionAI = true;
	UPROPERTY(EditAnywhere, Category = "Faction Properties") int _alienFactionQuantity = 2;
	UPROPERTY(EditAnywhere, Category = "Faction Properties") TSubclassOf<class AFactionController> factionControllerPrefab;

	UPROPERTY(EditAnywhere, Category = "Global", meta = (ClampMin = 0)) int32 startingResourceCap = 300;
	UPROPERTY(EditAnywhere, Category = "Global", meta = (ClampMin = 0)) int32 ticksTillHarvest = 8;
	UPROPERTY(EditAnywhere, Category = "Global", meta = (ClampMin = 0)) int32 popDeathsPerFoodMissing = 5;
	UPROPERTY(EditAnywhere, Category = "Global", meta = (ClampMin = 0)) int32 popDeathsPerPowerMissing = 5;
	UPROPERTY(EditAnywhere, Category = "Global", meta = (ClampMin = 0)) int32 daysTillStarve = 3;
	UPROPERTY(EditAnywhere, Category = "Global", meta = (ClampMin = 0)) int32 daysTillPowerOutage = 3;
	
	UPROPERTY(EditAnywhere, Category = "Human", meta = (ClampMin = 0)) TMap<EStratResources, int> humanStartResources;
	UPROPERTY(EditAnywhere, Category = "Alien", meta = (ClampMin = 0)) TMap<EStratResources, int> alienStartResources;

	UPROPERTY(EditAnywhere, Category = "Human") TMap<WorkerType, FWorkerStats> humanWorkers;
	UPROPERTY(EditAnywhere, Category = "Alien") TMap<WorkerType, FWorkerStats> alienWorkers;
};
