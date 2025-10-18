// Copyright Epic Games, Inc. All Rights Reserved.


#include "CapstoneProjectGameModeBase.h"

float ACapstoneProjectGameModeBase::currSeconds = 0.f;
FDateTickUpdate ACapstoneProjectGameModeBase::dateTickUpdates = FDateTickUpdate();

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

	factionColors.Add(Factions::None, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/Unclaimed.Unclaimed")));
	factionColors.Add(Factions::Human, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/HumanOwned.HumanOwned")));
	factionColors.Add(Factions::Alien1, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned01.AlienOwned01")));
	factionColors.Add(Factions::Alien2, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned02.AlienOwned02")));
	factionColors.Add(Factions::Alien3, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned03.AlienOwned03")));
	factionColors.Add(Factions::Alien4, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned04.AlienOwned04")));
	factionColors.Add(Factions::Alien5, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned05.AlienOwned05")));
	factionColors.Add(Factions::Alien6, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned06.AlienOwned06")));
}

void ACapstoneProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	gameState = GameStates::None;

	//Create initial player faction and assign it to the player controller
	CreateNewFaction();

	//Create alien factions, quantity determined by variable
	for (int i = 0; i < alienFactionQuantity; ++i)
	{
		CreateNewFaction();
	}

	for (auto& currentFaction : activeFactions)
	{
		currentFaction.Value->FindActiveFactions();
		currentFaction.Value->factionColor = factionColors[currentFaction.Key];
	}

	//For debug purposes, Human and Alien1 hate each other, Human and Alien2 are chill, and Alien3+ literally couldn't give less of a fuck
	//activeFactions[Factions::Human]->SetFactionRelationship(Factions::Alien1, FactionRelationship::Enemy);
	//activeFactions[Factions::Human]->SetFactionRelationship(Factions::Alien2, FactionRelationship::Ally);

	FActorSpawnParameters params;
	AGlobalSpawner::spawnerObject = GetWorld()->SpawnActor<AGlobalSpawner>(spawner, params);

	timeScale = 1.f;
	//FindExistingBuildingsAndTroops();
	//FindExistingHexes();
}

void ACapstoneProjectGameModeBase::Tick(float DeltaTime)
{
	Harvest(DeltaTime);
	Scan(DeltaTime);
	DateTick(DeltaTime);
	CheckHumanPop();
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

Factions ACapstoneProjectGameModeBase::CreateNewFaction()
{
	//Advance in the Factions enum, element 0 is None
	++factionCount;
	
	//Get the selected faction in the Factions enum
	Factions selectedFaction = static_cast<Factions>(factionCount);

	//Create a new Faction instance
	UFaction* newFaction = NewObject<UFaction>();
	newFaction->AddToRoot();

	AFactionController* factionController = selectedFaction == Factions::Human ? nullptr :
		GetWorld()->SpawnActor<AFactionController>(factionControllerPrefab ? factionControllerPrefab : AFactionController::StaticClass());

	//Make the new Faction instance aware of the faction it's assigned to
	newFaction->SetFaction(selectedFaction);
	newFaction->SetFoodAndDeathCosts(foodPerNonWorkers, foodPerWorkers, popDeathsPerFoodMissing, popDeathsPerPowerMissing);
	if (factionController)
	{
		newFaction->SetFactionController(factionController);
		factionController->SetFaction(newFaction);
	}

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
	for (auto& faction : activeFactions)
	{
		for (auto& resource : faction.Value->resourceInventory)
		{
			activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources += faction.Value->resourceInventory[resource.Key].resourcePerTick;

			activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources = FMath::Clamp(activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources, 0, activeFactions[faction.Key]->resourceInventory[resource.Key].maxResources);
		}
	}
	currentHarvestTime = harvestTickLength;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Food = %d, %d max, %d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Food].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Food].maxResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Food].resourcePerTick));
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Production = %d, %d max,%d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Production].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Production].maxResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Production].resourcePerTick));
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Energy = %d, %d max, %d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].maxResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].resourcePerTick));
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

void ACapstoneProjectGameModeBase::UpdateBuildings()
{
	for (auto& faction : activeFactions)
	{
		for (ABuilding* building : faction.Value->allBuildings)
		{
			building->HealOverTime();
		}
	}
}

void ACapstoneProjectGameModeBase::FeedPop()
{
	for (auto &faction : activeFactions)
	{
		faction.Value->FeedPop();
	}
}

void ACapstoneProjectGameModeBase::ConsumeEnergy()
{
	for (auto& faction : activeFactions)
	{
		faction.Value->ConsumeEnergy();
	}
}

void ACapstoneProjectGameModeBase::FindExistingBuildingsAndTroops()
{
	TArray<AActor*> buildings; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuilding::StaticClass(), buildings);

	for (int i = 0; i < buildings.Num(); i++)
	{
		ABuilding* building = Cast<ABuilding>(buildings[i]);
		if (FUnitData* data = building->GetUnitData())
		{
			Factions buildingFaction = data->GetFaction();
			if (activeFactions.Contains(buildingFaction))
			{
				activeFactions[buildingFaction]->allBuildings.Add(building);
			}
		}
	}

	TArray<AActor*> troops;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATroop::StaticClass(), troops);

	for (int i = 0; i < troops.Num(); i++)
	{
		ATroop* troop = Cast<ATroop>(troops[i]);
		Factions troopFaction = troop->GetUnitData()->GetFaction();
		if (activeFactions.Contains(troopFaction))
		{
			activeFactions[troopFaction]->allUnits.Add(troop);
		}
	}
}

void ACapstoneProjectGameModeBase::FindExistingHexes()
{
	TArray<AActor*> hexes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseHex::StaticClass(), hexes);

	for (int i = 0; i < hexes.Num(); i++)
	{
		ABaseHex* hex = Cast<ABaseHex>(hexes[i]);
	}
}

void ACapstoneProjectGameModeBase::UpdateResourceCosts()
{
	for (auto& faction : activeFactions)
	{
		faction.Value->UpdateResourceCosts();
	}
}

void ACapstoneProjectGameModeBase::CheckHumanPop()
{
	FWorkerStats workers = activeFactions[Factions::Human]->availableWorkers[WorkerType::Human];

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

void ACapstoneProjectGameModeBase::SpawnEnemies()
{
	if (blockEnemySpawning) return;

	for (auto& factionObject : activeFactions)
	{
		factionObject.Value->SpawnEnemy();
	}
}

void ACapstoneProjectGameModeBase::SpawnBuildings()
{
	for (auto& factionObject : activeFactions)
	{
		factionObject.Value->BuildRandomBuilding();
	}
}
