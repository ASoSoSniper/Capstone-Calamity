// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FactionEnum.h"
#include "WorkerStats.h"
#include "UnitActions.h"
#include "Types/CinematicObject.h"
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

	long long int totalDateTicks = 0;
	bool minuteTick = false;
	bool hourTick = false;
	bool dayTick = false;
	bool monthTick = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameReady);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDateTick, const FDateTickUpdate&, update);

class UFaction;
class APathingVisualizer;
class AEventSystemManager;
class AFactionController;
class ABasePlayerController;

UCLASS()
class CAPSTONEPROJECT_API ACapstoneProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACapstoneProjectGameModeBase();
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	static inline TMap<AActor*, FCinematicObject> cinematicObjects;

	static inline float timeScale = 1.f;
	static inline bool playerTroopsNeededForAISpawn = false;
	static inline float prevTime = 1.f;

	static inline float currentScanTime = 0;
	UPROPERTY(EditAnywhere) float visibilityScanRate = 0.1f;

	UPROPERTY(EditAnywhere) int alienFactionQuantity = 2;
	static inline GameStates gameState;

	UFUNCTION(BlueprintCallable) float GetDeltaTime();
	UFUNCTION(BlueprintCallable) static void SetDeltaTime(float deltaTime);
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

	static FOnGameReady onGameReady;
	static FOnDateTick onDateTick;

private:

	FString currentDate;
	static FDateTickUpdate dateTickUpdates;
	static float currSeconds;

	void Scan(float& DeltaTime);

	void CheckHumanPop();
	void CheckDate();

	UPROPERTY(EditAnywhere) bool blockEnemySpawning = false;
};
