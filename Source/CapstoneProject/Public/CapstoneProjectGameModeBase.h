// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Faction.h"
#include "FactionEnum.h"
#include "BasePlayerController.h"
#include "CapstoneProjectGameModeBase.generated.h"

/**
 * 
 */

UCLASS()
class CAPSTONEPROJECT_API ACapstoneProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACapstoneProjectGameModeBase();
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	static inline TMap<Factions, Faction*> activeFactions;

	static inline float timeScale = 1.f;

	static inline float currentHarvestTime = 0;
	UPROPERTY(EditAnywhere) float harvestTickLength = 2.f;
	static inline float currentScanTime = 0;
	UPROPERTY(EditAnywhere) float visibilityScanRate = 0.1f;
	
	UPROPERTY(EditAnywhere) int alienFactionQuantity = 3;

	UPROPERTY(EditAnywhere) TSubclassOf<class AGlobalSpawner> spawner;
	static inline TArray<TerrainType> nonBuildableTerrains;


	UFUNCTION(BlueprintCallable) float GetDeltaTime();
	UFUNCTION(BlueprintCallable) void SetDeltaTime(float deltaTime);

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

	struct DayStruct
	{
		int currentMonth;
		int day;
		int hour;
		int minute;		
	};
	DayStruct dayStruct = DayStruct{0,1,0,0};
	float currSeconds = 0;

private:
	Factions CreateNewFaction();
	int factionCount = 0;

	void Harvest(float& DeltaTime);
	void Scan(float& DeltaTime);
};
