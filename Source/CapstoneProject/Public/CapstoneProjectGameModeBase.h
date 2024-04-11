// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Faction.h"
#include "FactionEnum.h"
#include "BasePlayerController.h"
#include "CapstoneProjectGameModeBase.generated.h"

UENUM(BlueprintType)
enum class GameStates : uint8
{
	None,
	Victory,
	Defeat
};

USTRUCT(BlueprintType, Blueprintable)
struct FDayStruct
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int currentMonth = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int day = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int hour = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int minute = 0;
};

UCLASS()
class CAPSTONEPROJECT_API ACapstoneProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACapstoneProjectGameModeBase();
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	static inline TMap<Factions, Faction*> activeFactions;
	static inline TMap<Factions, UMaterialInterface*> factionColors;

	static inline float timeScale = 1.f;
	float prevTime = 1.f;

	static inline float currentHarvestTime = 0;
	UPROPERTY(EditAnywhere) float harvestTickLength = 8.f;
	static inline float currentScanTime = 0;
	UPROPERTY(EditAnywhere) float visibilityScanRate = 0.1f;

	UPROPERTY(EditAnywhere) int popDeathsPerFoodMissing = 5;
	UPROPERTY(EditAnywhere) int popDeathsPerPowerMissing = 5;
	
	UPROPERTY(EditAnywhere) int alienFactionQuantity = 3;

	UPROPERTY(EditAnywhere) TSubclassOf<class AGlobalSpawner> spawner;
	UPROPERTY() AGlobalSpawner* spawnedSpawner;
	static inline TArray<TerrainType> nonBuildableTerrains;
	static inline GameStates gameState;

	UFUNCTION(BlueprintCallable) float GetDeltaTime();
	UFUNCTION(BlueprintCallable) void SetDeltaTime(float deltaTime);
	UFUNCTION(BlueprintCallable) void SetToPreviousTime();
	UFUNCTION(BlueprintCallable) bool TimeScaleIsZero();

	UFUNCTION(BlueprintCallable) FString Date(float& deltaTime);
	struct MonthStruct 
	{
		FString name;
		int numOfDays;
		int monthOfYear;
	};
	enum MonthEnum
	{
		Jan,
		Feb,
		Mar,
		Apr,
		May,
		Jun,
		Jul,
		Aug,
		Sep,
		Oct,
		Nov,
		Dec
	};
	TMap<MonthEnum, MonthStruct> MonthDic;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDayStruct dayStruct = FDayStruct{};
	float currSeconds = 0;

	UFUNCTION(BlueprintCallable) GameStates GetGameState();

	UFUNCTION(BlueprintCallable) FDayStruct GetDateInfo();

private:
	Factions CreateNewFaction();
	int factionCount = 0;

	void Harvest(float& DeltaTime);
	void Scan(float& DeltaTime);

	void UpdateBuildingOccupations();

	void FeedPop();
	void StarvePop(Factions faction, int foodCost);

	void ConsumeEnergy();
	void PowerOutage(Factions faction, int energyCost);

	void KillPopulation(Factions faction, int cost, int deathsPerResource);

	void RemoveWorkers(Factions faction, WorkerType worker);
	void FindExistingBuildingsAndTroops();
	void FindExistingHexes();
	void UpdateResourceCosts();

	void CheckHumanPop();
	void CheckDate();

	void SpawnEnemies();
	UPROPERTY(EditAnywhere) int daysTillArmySpawn = 1;
	int currentDaysTillArmySpawn = 0;
	UPROPERTY(VisibleAnywhere) bool playerBuiltTroop = false;

	UPROPERTY(EditAnywhere) int daysTillArmyGrowth = 10;
	int currentDaysTillArmyGrowth = 0;
	UPROPERTY(EditAnywhere) int maxTroopsInArmy = 20;
	int troopsInArmy = 0;
};
