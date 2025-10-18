// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Faction.h"
#include "FactionEnum.h"
#include "BasePlayerController.h"
#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FDayStruct
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int currentMonth = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int day = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int hour = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int minute = 0;
};

USTRUCT()
struct FDateTickUpdate
{
	GENERATED_USTRUCT_BODY()

	bool minuteTick = false;
	bool hourTick = false;
	bool dayTick = false;
	bool monthTick = false;
};

UCLASS()
class CAPSTONEPROJECT_API ACapstoneProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACapstoneProjectGameModeBase();
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	static inline TMap<Factions, UFaction*> activeFactions;
	static inline TMap<Factions, UMaterialInterface*> factionColors;

	static inline TMap<AActor*, FCinematicObject> cinematicObjects;

	static inline float timeScale = 1.f;
	static inline bool playerTroopsNeededForAISpawn = false;
	float prevTime = 1.f;

	static inline float currentHarvestTime = 0;
	UPROPERTY(EditAnywhere) float harvestTickLength = 8.f;
	static inline float currentScanTime = 0;
	UPROPERTY(EditAnywhere) float visibilityScanRate = 0.1f;

	UPROPERTY(EditAnywhere) int alienFactionQuantity = 2;
	UPROPERTY(EditAnywhere) TSubclassOf<class AGlobalSpawner> spawner;
	static inline GameStates gameState;

	UFUNCTION(BlueprintCallable) float GetDeltaTime();
	UFUNCTION(BlueprintCallable) void SetDeltaTime(float deltaTime);
	UFUNCTION(BlueprintCallable) void SetToPreviousTime();
	UFUNCTION(BlueprintCallable) bool TimeScaleIsZero();

	UFUNCTION() void DateTick(float& deltaTime);
	static FDateTickUpdate* GetDateUpdates();
	UFUNCTION(BlueprintCallable, BlueprintPure) FString GetWorldDate() const;
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
	

	UFUNCTION(BlueprintCallable) GameStates GetGameState();

	static float GetTimeTillNextTick();

private:

	FString currentDate;
	static FDateTickUpdate dateTickUpdates;
	static float currSeconds;

	Factions CreateNewFaction();
	int factionCount = 0;

	void Harvest(float& DeltaTime);
	void Scan(float& DeltaTime);

	void UpdateBuildings();

	void UpdateResourceCosts();
	void FeedPop();
	void ConsumeEnergy();

	void FindExistingBuildingsAndTroops();
	void FindExistingHexes();

	void CheckHumanPop();
	void CheckDate();

	void SpawnEnemies();
	void SpawnBuildings();
	UPROPERTY(EditAnywhere) bool blockEnemySpawning = false;
	UPROPERTY(EditAnywhere, Category = "Faction") TSubclassOf<class AFactionController> factionControllerPrefab;
	UPROPERTY(EditAnywhere, Category = "Worker Costs") int foodPerNonWorkers = 10;
	UPROPERTY(EditAnywhere, Category = "Worker Costs") int foodPerWorkers = 5;
	UPROPERTY(EditAnywhere, Category = "Worker Costs") int popDeathsPerFoodMissing = 5;
	UPROPERTY(EditAnywhere, Category = "Worker Costs") int popDeathsPerPowerMissing = 5;
};
