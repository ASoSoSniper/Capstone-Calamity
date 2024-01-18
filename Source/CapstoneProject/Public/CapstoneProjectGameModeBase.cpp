// Copyright Epic Games, Inc. All Rights Reserved.


#include "CapstoneProjectGameModeBase.h"

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
	dayStruct.currentMonth = 0;
	currSeconds = 1;

	nonBuildableTerrains.Add(TerrainType::AlienCity);
	nonBuildableTerrains.Add(TerrainType::Ship);
	nonBuildableTerrains.Add(TerrainType::TheRock);
	nonBuildableTerrains.Add(TerrainType::Mountains);
	nonBuildableTerrains.Add(TerrainType::None);
}

void ACapstoneProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	end = false;

	//Create initial player faction and assign it to the player controller
	CreateNewFaction();

	//Create alien factions, quantity determined by variable
	for (int i = 0; i < alienFactionQuantity; ++i)
	{
		CreateNewFaction();
	}

	for (auto currentFaction : activeFactions)
	{
		currentFaction.Value->FindActiveFactions();
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("%d factions created!"), activeFactions.Num()));

	//For debug purposes, Human and Alien1 hate each other, Human and Alien2 are chill, and Alien3+ literally couldn't give less of a fuck
	activeFactions[Factions::Human]->factionRelationships[Factions::Alien1] = FactionRelationship::Enemy;
	activeFactions[Factions::Alien1]->factionRelationships[Factions::Human] = FactionRelationship::Enemy;

	activeFactions[Factions::Human]->factionRelationships[Factions::Alien2] = FactionRelationship::Ally;
	activeFactions[Factions::Alien2]->factionRelationships[Factions::Human] = FactionRelationship::Ally;

	FActorSpawnParameters params;
	GetWorld()->SpawnActor<AGlobalSpawner>(spawner, params);

	timeScale = 0.f;
}

void ACapstoneProjectGameModeBase::Tick(float DeltaTime)
{
	Harvest(DeltaTime);
	Scan(DeltaTime);
	Date(DeltaTime);
}



float ACapstoneProjectGameModeBase::GetDeltaTime()
{
	return timeScale;
}

void ACapstoneProjectGameModeBase::SetDeltaTime(float deltaTime)
{
	timeScale = deltaTime;
}

FString ACapstoneProjectGameModeBase::Date(float& deltaTime)
{
	FText extraMinuteZero = FText::FromString("");
	FText extraHourZero = FText::FromString("");
	FText extraDayZero = FText::FromString("");

	if (currSeconds < 1.f)
	{
		currSeconds += deltaTime * timeScale;
	}
	else
	{
		currSeconds = 0.f;

		if (dayStruct.minute == 0)
		{
			dayStruct.minute = 30;
		}
		else
		{
			dayStruct.minute = 0;

			if (dayStruct.hour < 23)
			{
				dayStruct.hour += 1;
			}
			else
			{
				dayStruct.hour = 0;

				if (dayStruct.day < MonthDic[MonthEnum(dayStruct.currentMonth)].numOfDays)
				{
					dayStruct.day += 1;
				}
				else
				{
					dayStruct.day = 1;
					
					if (dayStruct.currentMonth < 12)
					{
						dayStruct.currentMonth += 1;
					}
					else
					{
						dayStruct.currentMonth = 1;
					}
				}
			}
		}
	}
	if (dayStruct.minute < 10) extraMinuteZero = FText::FromString("0");
	if (dayStruct.hour < 10) extraHourZero = FText::FromString("0");
	if (dayStruct.day < 10) extraDayZero = FText::FromString("0");

	FString extraDayZeroString = extraDayZero.ToString();
	FString extraHourZeroString = extraHourZero.ToString();
	FString extraMinuteZeroString = extraMinuteZero.ToString();

	TArray<FStringFormatArg> Args;
	Args.Add(FStringFormatArg(MonthDic[MonthEnum(dayStruct.currentMonth)].name));
	Args.Add(FStringFormatArg(extraDayZeroString));
	Args.Add(FStringFormatArg(dayStruct.day));
	Args.Add(FStringFormatArg(extraHourZeroString));
	Args.Add(FStringFormatArg(dayStruct.hour));
	Args.Add(FStringFormatArg(extraMinuteZeroString));
	Args.Add(FStringFormatArg(dayStruct.minute));
	return FString::Format(TEXT("{0} {1}{2}  -  {3}{4}:{5}{6}"), Args);
}

bool ACapstoneProjectGameModeBase::HasEnded()
{
	return end;
}

FDayStruct ACapstoneProjectGameModeBase::GetDateInfo()
{
	return FDayStruct();
}

Factions ACapstoneProjectGameModeBase::CreateNewFaction()
{
	//Advance in the Factions enum, element 0 is None
	++factionCount;
	
	//Get the selected faction in the Factions enum
	Factions selectedFaction = static_cast<Factions>(factionCount);

	//Create a new Faction instance
	Faction* newFaction = new Faction();

	//Make the new Faction instance aware of the faction it's assigned to
	newFaction->faction = selectedFaction;

	//In the global faction dictionary, add the selected faction as a new key and the Faction instance as its value
	activeFactions.Add(selectedFaction, newFaction);

	//Return the selected faction
	return selectedFaction;
}

void ACapstoneProjectGameModeBase::Harvest(float& DeltaTime)
{
	if (currentHarvestTime > 0)
	{
		currentHarvestTime -= DeltaTime * timeScale;
		return;
	}
	for (auto faction : activeFactions)
	{
		for (auto resource : faction.Value->resourceInventory)
		{
			activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources += faction.Value->resourceInventory[resource.Key].resourcePerTick;

			activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources = FMath::Clamp(activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources, 0, activeFactions[faction.Key]->resourceInventory[resource.Key].maxResources);
		}
	}
	currentHarvestTime = harvestTickLength;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Food = %d, %d max, %d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Food].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Food].maxResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Food].resourcePerTick));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Production = %d, %d max,%d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Production].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Production].maxResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Production].resourcePerTick));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Energy = %d, %d max, %d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].maxResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].resourcePerTick));
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
