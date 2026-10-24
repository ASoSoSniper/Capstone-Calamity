// Copyright Epic Games, Inc. All Rights Reserved.


#include "CapstoneProjectGameModeBase.h"
#include "BasePlayerController.h"
#include "Faction.h"
#include "EventSystemManager.h"
#include "PathingVisualizer.h"
#include "FactionController.h"
#include "Types/UnitData.h"
#include "Managers/FactionManager.h"

float ACapstoneProjectGameModeBase::currSeconds = 0.f;
FDateTickUpdate ACapstoneProjectGameModeBase::dateTickUpdates = FDateTickUpdate();
FOnGameReady ACapstoneProjectGameModeBase::onGameReady = FOnGameReady();
FOnDateTick ACapstoneProjectGameModeBase::onDateTick = FOnDateTick();

ACapstoneProjectGameModeBase::ACapstoneProjectGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	MonthDic.Add(Jan, MonthStruct{ TEXT("JAN"), 31, 1 });
	MonthDic.Add(Feb, MonthStruct{ TEXT("FEB"), 28, 2 });
	MonthDic.Add(Mar, MonthStruct{ TEXT("MAR"), 30, 3 });
	MonthDic.Add(Apr, MonthStruct{ TEXT("APR"), 31, 4 });
	MonthDic.Add(May, MonthStruct{ TEXT("MAY"), 30, 5 });
	MonthDic.Add(Jun, MonthStruct{ TEXT("JUN"), 31, 6 });
	MonthDic.Add(Jul, MonthStruct{ TEXT("JUL"), 30, 7 });
	MonthDic.Add(Aug, MonthStruct{ TEXT("AUG"), 31, 8 });
	MonthDic.Add(Sep, MonthStruct{ TEXT("SEP"), 30, 9 });
	MonthDic.Add(Oct, MonthStruct{ TEXT("OCT"), 31, 10 });
	MonthDic.Add(Nov, MonthStruct{ TEXT("NOV"), 30, 11 });
	MonthDic.Add(Dec, MonthStruct{ TEXT("DEC"), 31, 12 });
	dayStruct.currentMonth = 8;
	dayStruct.day = 26;
	currSeconds = 1;
}

void ACapstoneProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	gameState = GameStates::None;
	onDateTick.Clear();

	timeScale = 1.f;
	dateTickUpdates.totalDateTicks = 0;
	dateTickUpdates.minuteTick = false;
	dateTickUpdates.hourTick = false;
	dateTickUpdates.dayTick = false;
	dateTickUpdates.monthTick = false;

	onGameReady.Broadcast();
}

void ACapstoneProjectGameModeBase::Tick(float DeltaTime)
{
	if (!AFactionManager::Initialized()) return;

	Scan(DeltaTime);
	DateTick(DeltaTime);
	//CheckHumanPop();
	CheckDate();

	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("Enemy has %d targets"), activeFactions[Factions::Alien1]->targetList.Num()));
}

float ACapstoneProjectGameModeBase::GetDeltaTime()
{
	return timeScale;
}

void ACapstoneProjectGameModeBase::SetDeltaTime(float deltaTime)
{
	if (deltaTime == 0.f) prevTime = timeScale;

	timeScale = deltaTime;
}

void ACapstoneProjectGameModeBase::SetToPreviousTime()
{
	timeScale = prevTime;
}

bool ACapstoneProjectGameModeBase::TimeScaleIsZero()
{
	return timeScale == 0.f;
}

void ACapstoneProjectGameModeBase::DateTick(float& deltaTime)
{
	dateTickUpdates.minuteTick = false;
	dateTickUpdates.hourTick = false;
	dateTickUpdates.dayTick = false;
	dateTickUpdates.monthTick = false;

	//Count up in seconds, scaled with timeScale
	currSeconds += deltaTime * timeScale;

	//If 1 second of scaled time passes, update world time by 30 minutes
	if (currSeconds > 1.f)
	{
		currSeconds = 0.f;
		dayStruct.minute = dayStruct.minute == 0 ? 30 : 0;
		dateTickUpdates.minuteTick = true;
	}

	//If the minute display was updated, trigger behaviors
	if (dateTickUpdates.minuteTick)
	{
		//Update hour when minutes reset to 0
		if (dayStruct.minute == 0) dateTickUpdates.hourTick = true;
	}

	//If the hour display was updated, trigger behaviors
	if (dateTickUpdates.hourTick)
	{
		dayStruct.hour++;

		//Update day when hours reset to 0
		if (dayStruct.hour > 23)
		{
			dayStruct.hour = 0;
			dateTickUpdates.dayTick = true;
		}
	}

	//If the day display was updated, trigger behaviors
	if (dateTickUpdates.dayTick)
	{
		dayStruct.day++;

		//Update month when days reset to 1
		if (dayStruct.day > MonthDic[MonthEnum(dayStruct.currentMonth)].numOfDays)
		{
			dayStruct.day = 1;
			dateTickUpdates.monthTick = true;
		}
	}

	//If month display was updated, trigger behaviors
	if (dateTickUpdates.monthTick)
	{
		dayStruct.currentMonth++;
		if (dayStruct.currentMonth > 12)
		{
			dayStruct.currentMonth = 1;
		}
	}

	FString extraMinuteZero = dayStruct.minute < 10 ? TEXT("0") : TEXT("");
	FString extraHourZero = dayStruct.hour < 10 ? TEXT("0") : TEXT("");
	FString extraDayZero = dayStruct.day < 10 ? TEXT("0") : TEXT("");

	TArray<FStringFormatArg> Args;
	Args.Add(FStringFormatArg(MonthDic[MonthEnum(dayStruct.currentMonth)].name));
	Args.Add(FStringFormatArg(extraDayZero));
	Args.Add(FStringFormatArg(dayStruct.day));
	Args.Add(FStringFormatArg(extraHourZero));
	Args.Add(FStringFormatArg(dayStruct.hour));
	Args.Add(FStringFormatArg(extraMinuteZero));
	Args.Add(FStringFormatArg(dayStruct.minute));
	currentDate = FString::Format(TEXT("{0} {1}{2}  -  {3}{4}:{5}{6}"), Args);

	if (dateTickUpdates.minuteTick)
	{
		dateTickUpdates.totalDateTicks++;
		onDateTick.Broadcast(dateTickUpdates);
	}
}

FDateTickUpdate* ACapstoneProjectGameModeBase::GetDateUpdates()
{
	return &dateTickUpdates;
}

FString ACapstoneProjectGameModeBase::GetWorldDate() const
{
	return currentDate;
}

GameStates  ACapstoneProjectGameModeBase::GetGameState()
{
	return gameState;
}

float ACapstoneProjectGameModeBase::GetTimeTillNextTick()
{
	return FMath::Clamp(1.f - currSeconds, 0.f, 1.f);
}

void ACapstoneProjectGameModeBase::Scan(float& DeltaTime)
{
	if (currentScanTime > 0)
	{
		currentScanTime -= DeltaTime;
		return;
	}
	currentScanTime = visibilityScanRate;
}

void ACapstoneProjectGameModeBase::CheckHumanPop()
{
	FWorkerStats workers = AFactionManager::GetFactionObject(EFactions::Human)->availableWorkers[WorkerType::Organic];

	if (workers.available + workers.working < 25)
	{
		gameState = GameStates::Defeat;
	}
}

void ACapstoneProjectGameModeBase::CheckDate()
{
	if (dayStruct.currentMonth != 8) return;

	if (dayStruct.day != 25) return;

	gameState = GameStates::Defeat;
}
