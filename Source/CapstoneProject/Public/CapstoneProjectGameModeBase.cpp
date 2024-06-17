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
	dayStruct.currentMonth = 8;
	dayStruct.day = 26;
	currSeconds = 1;

	nonBuildableTerrains.Add(TerrainType::AlienCity);
	//nonBuildableTerrains.Add(TerrainType::Ship);
	nonBuildableTerrains.Add(TerrainType::TheRock);
	nonBuildableTerrains.Add(TerrainType::Mountains);
	nonBuildableTerrains.Add(TerrainType::Jungle);
	nonBuildableTerrains.Add(TerrainType::None);

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
	spawnedSpawner = GetWorld()->SpawnActor<AGlobalSpawner>(spawner, params);

	timeScale = 1.f;
	FindExistingBuildingsAndTroops();
	FindExistingHexes();
}

void ACapstoneProjectGameModeBase::Tick(float DeltaTime)
{
	Harvest(DeltaTime);
	Scan(DeltaTime);
	Date(DeltaTime);
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
			UpdateBuildings();
		}
		else
		{
			dayStruct.minute = 0;
			UpdateBuildings();

			if (dayStruct.hour < 23)
			{
				dayStruct.hour += 1;
				UpdateResourceCosts();
			}
			else
			{
				dayStruct.hour = 0;
				UpdateResourceCosts();

				if (dayStruct.day < MonthDic[MonthEnum(dayStruct.currentMonth)].numOfDays)
				{
					dayStruct.day += 1;
					
					FeedPop();
					ConsumeEnergy();
					SpawnEnemies();
				}
				else
				{
					dayStruct.day = 1;
					
					FeedPop();
					ConsumeEnergy();
					SpawnEnemies();
					
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

GameStates  ACapstoneProjectGameModeBase::GetGameState()
{
	return gameState;
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
	newFaction->SetFaction(selectedFaction);

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
		int workerAvailableCost = 0;
		int workerFoodCost = 0;

		CalculateFoodCosts(faction.Key, workerAvailableCost, workerFoodCost);

		int totalCost = activeFactions[faction.Key]->resourceInventory[StratResources::Food].lossesPerDay;
		
		//Enter full starvation if unaffordable non-working food cost
		if (activeFactions[faction.Key]->resourceInventory[StratResources::Food].currentResources < workerAvailableCost)
		{
			StarvePop(faction.Key, totalCost);
			return;
		}
		activeFactions[faction.Key]->resourceInventory[StratResources::Food].currentResources -= workerAvailableCost;

		//Enter worker starvation if unaffordable working food cost
		if (activeFactions[faction.Key]->resourceInventory[StratResources::Food].currentResources < workerFoodCost)
		{
			StarvePop(faction.Key, totalCost);
			return;
		}
		activeFactions[faction.Key]->resourceInventory[StratResources::Food].currentResources -= workerFoodCost;

		activeFactions[faction.Key]->currStarveDays = 0;
		activeFactions[faction.Key]->starving = false;
	}
}

void ACapstoneProjectGameModeBase::StarvePop(Factions faction, int foodCost)
{
	int missingFood = foodCost - activeFactions[faction]->resourceInventory[StratResources::Food].currentResources;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Food cost = %d"), missingFood));

	activeFactions[faction]->resourceInventory[StratResources::Food].currentResources = 0;	

	if (!activeFactions[faction]->starving)
	{
		activeFactions[faction]->starving = true;
		//RemoveWorkers(faction);
	}

	if (activeFactions[faction]->currStarveDays < activeFactions[faction]->daysTillStarve)
	{
		++activeFactions[faction]->currStarveDays;
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%d days remaining till starve"), activeFactions[faction]->daysTillStarve - activeFactions[faction]->currStarveDays));
		return;
	}

	KillPopulation(faction, missingFood, popDeathsPerFoodMissing);
}

void ACapstoneProjectGameModeBase::ConsumeEnergy()
{
	for (auto& faction : activeFactions)
	{
		int energyCost = activeFactions[faction.Key]->resourceInventory[StratResources::Energy].lossesPerDay;

		if (energyCost > faction.Value->resourceInventory[StratResources::Energy].currentResources)
		{
			PowerOutage(faction.Key, energyCost);
			RemoveWorkers(faction.Key, WorkerType::Robot);
			return;
		}

		faction.Value->resourceInventory[StratResources::Energy].currentResources -= energyCost;
		faction.Value->currPowerDays = 0;
		faction.Value->powerOutage = false;
		if (faction.Key == Factions::Human) UnitActions::EnableRobots(Factions::Human, true);
	}
}

void ACapstoneProjectGameModeBase::PowerOutage(Factions faction, int energyCost)
{
	int missingEnergy = energyCost - activeFactions[faction]->resourceInventory[StratResources::Energy].currentResources;	

	if (!activeFactions[faction]->powerOutage)
	{
		activeFactions[faction]->powerOutage = true;
		activeFactions[faction]->resourceInventory[StratResources::Energy].currentResources = 0;
		if (faction == Factions::Human) UnitActions::EnableRobots(Factions::Human, false);
	}

	if (activeFactions[faction]->currPowerDays < activeFactions[faction]->daysTillPowerOutage)
	{
		++activeFactions[faction]->currPowerDays;
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%d days remaining till power outage"), activeFactions[faction]->daysTillPowerOutage - activeFactions[faction]->currPowerDays));
		return;
	}

	if (faction == Factions::Human) UnitActions::EnableRobots(Factions::Human, true);
	KillPopulation(faction, missingEnergy, popDeathsPerPowerMissing);
}

void ACapstoneProjectGameModeBase::KillPopulation(Factions faction, int cost, int deathsPerResource)
{
	int remainingCost = 0;

	//Kill non-working population
	activeFactions[faction]->availableWorkers[WorkerType::Human].available -= cost * deathsPerResource;

	if (activeFactions[faction]->availableWorkers[WorkerType::Human].available < 0)
	{
		remainingCost = -1 * activeFactions[faction]->availableWorkers[WorkerType::Human].available;
		activeFactions[faction]->availableWorkers[WorkerType::Human].available = 0;
	}
	else return;

	//Kill working population
	TArray<ABaseHex*> hexesWithWorkers;
	for (ABaseHex* hex : ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes)
	{
		if (hex->workersInHex[WorkerType::Human] > 0) hexesWithWorkers.Add(hex);
	}

	int scanIndex = 0;
	int workersToRemove = FMath::Min(remainingCost, activeFactions[faction]->availableWorkers[WorkerType::Human].working);

	if (hexesWithWorkers.IsEmpty()) return;

	int overloadStopper = 0;
	while (workersToRemove != 0)
	{
		if (hexesWithWorkers[scanIndex]->workersInHex[WorkerType::Human] > 0)
		{
			hexesWithWorkers[scanIndex]->workersInHex[WorkerType::Human]--;
			workersToRemove--;
		}

		scanIndex++;
		if (!hexesWithWorkers.IsValidIndex(scanIndex))
		{
			scanIndex = 0;
		}

		overloadStopper++;
		if (overloadStopper == 100)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not finish"));
			return;
		}
	}
	activeFactions[faction]->availableWorkers[WorkerType::Human].working -= remainingCost;

	if (activeFactions[faction]->availableWorkers[WorkerType::Human].working < 0)
		activeFactions[faction]->availableWorkers[WorkerType::Human].working = 0;
}

void ACapstoneProjectGameModeBase::RemoveWorkers(Factions faction, WorkerType worker)
{
	for (ABaseHex* hex : activeFactions[faction]->ownedHexes)
	{
		activeFactions[faction]->availableWorkers[worker].available += hex->workersInHex[worker];
		hex->workersInHex[worker] = 0;
	}
}

void ACapstoneProjectGameModeBase::FindExistingBuildingsAndTroops()
{
	TArray<AActor*> buildings; 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuilding::StaticClass(), buildings);

	for (int i = 0; i < buildings.Num(); i++)
	{
		ABuilding* building = Cast<ABuilding>(buildings[i]);
		Factions buildingFaction = building->unitStats->faction;
		if (activeFactions.Contains(buildingFaction))
		{
			activeFactions[buildingFaction]->allBuildings.Add(building);
		}
	}

	TArray<AActor*> troops;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATroop::StaticClass(), troops);

	for (int i = 0; i < troops.Num(); i++)
	{
		ATroop* troop = Cast<ATroop>(troops[i]);
		Factions troopFaction = troop->unitStats->faction;
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
		hex->SetFaction(hex->hexOwner);
	}
}

void ACapstoneProjectGameModeBase::UpdateResourceCosts()
{
	for (auto& faction : activeFactions)
	{
		//Food
		int workerAvailableCost = 0;
		int workerFoodCost = 0;

		CalculateFoodCosts(faction.Key, workerAvailableCost, workerFoodCost);
		faction.Value->resourceInventory[StratResources::Food].lossesPerDay = workerAvailableCost + workerFoodCost;

		//Energy
		int energyCost = CalculateEnergyCosts(faction.Key);
		
		faction.Value->resourceInventory[StratResources::Energy].lossesPerDay = energyCost;
	}
}

void ACapstoneProjectGameModeBase::CalculateFoodCosts(Factions faction, int& availableWorkerCost, int& workingWorkerCost)
{
	int remainder = activeFactions[faction]->availableWorkers[WorkerType::Human].available % foodPerNonWorkers;
	availableWorkerCost = activeFactions[faction]->availableWorkers[WorkerType::Human].available / foodPerNonWorkers;
	availableWorkerCost = (availableWorkerCost + (remainder == 0 ? 0 : 1));

	int workerRemainder = 0;

	for (auto& workers : activeFactions[faction]->availableWorkers)
	{
		workerRemainder = workers.Value.working % foodPerWorkers;
		int cost = workers.Value.working / foodPerWorkers;
		workingWorkerCost += (cost + (workerRemainder == 0 ? 0 : 1)) * workers.Value.workingFoodCost;
	}
}

int ACapstoneProjectGameModeBase::CalculateEnergyCosts(Factions faction)
{
	int energyCost = 0;

	if (!activeFactions[faction]->powerOutage)
	{
		for (ATroop* troop : activeFactions[faction]->allUnits)
		{
			energyCost += troop->unitStats->energyUpkeep;
		}
	}
	for (ABuilding* building : activeFactions[faction]->allBuildings)
	{
		energyCost += building->unitStats->energyUpkeep;
	}
	for (ABaseHex* hex : ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes)
	{
		energyCost += hex->workersInHex[WorkerType::Robot] * UnitActions::GetWorkerEnergyCost(faction)[WorkerType::Robot];
	}

	return energyCost;
}

void ACapstoneProjectGameModeBase::CheckHumanPop()
{
	Faction::WorkerStats workers = activeFactions[Factions::Human]->availableWorkers[WorkerType::Human];

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

	--currentDaysTillArmySpawn;
	--currentDaysTillArmyGrowth;

	if (currentDaysTillArmyGrowth <= 0)
	{
		troopsInArmy = FMath::Clamp(troopsInArmy + 1, 1, maxTroopsInArmy);
		currentDaysTillArmyGrowth = daysTillArmyGrowth;
	}

	if (currentDaysTillArmySpawn > 0) return;

	TArray<AActor*> troopsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATroop::StaticClass(), troopsFound);

	TArray<ATroop*> enemyTroops;
	for (int i = 0; i < troopsFound.Num(); i++)
	{
		if (ATroop* troop = Cast<ATroop>(troopsFound[i]))
		{
			if (troop->unitStats->faction != Factions::Human)
			{
				enemyTroops.Add(troop);
			}
			else
			{
				playerBuiltTroop = true;
			}
		}
	}

	if (!playerBuiltTroop) return;

	TArray<ABaseHex*> spawnableHexes;
	for (int i = 0; i < spawnedSpawner->alienHexes.Num(); i++)
	{
		ABuilding* building = spawnedSpawner->alienHexes[i]->building;
		if (building)
		{
			if (spawnedSpawner->alienHexes[i]->battle) continue;
			if (building->siegingFaction != Factions::None) continue;
		}

		spawnableHexes.Add(spawnedSpawner->alienHexes[i]);
	}

	if (!spawnableHexes.IsEmpty() && enemyTroops.Num() < spawnableHexes.Num())
	{
		ABaseHex* randHex = spawnedSpawner->alienHexes[FMath::RandRange(0, spawnableHexes.Num() - 1)];

		ATroop* army = spawnedSpawner->BuildArmy(Factions::Alien1, randHex);
		army->AITroopComponent->troopsInArmy = troopsInArmy;

		currentDaysTillArmySpawn = daysTillArmySpawn;
	}
}
