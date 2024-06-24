// Fill out your copyright notice in the Description page of Project Settings.


#include "Faction.h"
#include "CapstoneProjectGameModeBase.h"


Faction::Faction()
{
	faction = Factions::None;
	resourceInventory.Add(StratResources::Energy, InventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(StratResources::Production, InventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(StratResources::Food, InventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(StratResources::Wealth, InventoryStat{ 000, 300, 0, 0 });

	availableWorkers.Add(WorkerType::Human, WorkerStats{ 0,500, 0,1,0 });
	availableWorkers.Add(WorkerType::Robot, WorkerStats{ 0,10, 1,0,0 });
	availableWorkers.Add(WorkerType::Alien, WorkerStats{ 0,0, 0,1,0 });

	armyNamesHuman.Add(TEXT("Fuckers"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Asswipes"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Shitstains"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Muppets"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Dummies"), TArray<int32>());
	armyNamesHuman.Add(TEXT("NPCs"), TArray<int32>());

	armyNamesAlien.Add(TEXT("Uncrustables"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Oatmeal Cream Pies"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Hot Pockets"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Pop Tarts"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Quakers"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Lucky Charms"), TArray<int32>());
}

Faction::~Faction()
{
}

void Faction::FindActiveFactions()
{
	for (auto& currentFaction : ACapstoneProjectGameModeBase::activeFactions)
	{
		if (currentFaction.Key != faction)
		{
			FactionRelationship relationShip = FactionRelationship::Neutral;

			factionRelationships.Add(currentFaction.Key, FRelationshipStats{ relationShip });
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Faction found %d other factions!"), factionRelationships.Num()));
}

void Faction::SetFoodAndDeathCosts(int foodPerNonWorkersVar, int foodPerWorkersVar, int popDeathsPerFoodMissingVar, int popDeathsPerPowerMissingVar)
{
	foodPerNonWorkers = foodPerNonWorkersVar;
	foodPerWorkers = foodPerWorkersVar;

	popDeathsPerFoodMissing = popDeathsPerFoodMissingVar;
	popDeathsPerPowerMissing = popDeathsPerPowerMissingVar;
}

FactionRelationship Faction::GetFactionRelationship(Factions targetFaction)
{
	//If comparing unit faction to itself, return Ally.
	if (faction == targetFaction) return FactionRelationship::Ally;

	//If unit faction has a relationship with target faction, return that relationship.
	if (factionRelationships.Contains(targetFaction))
	{
		return factionRelationships[targetFaction].relationship;
	}

	//Otherwise, return Neutral.
	return FactionRelationship::Neutral;
}

FactionRelationship Faction::GetFactionRelationship(AActor* target)
{
	UUnitStats* stats = target->FindComponentByClass<UUnitStats>();

	if (stats) return GetFactionRelationship(stats->faction);

	return FactionRelationship::Neutral;
}

void Faction::SetFactionRelationship(Factions targetFaction, FactionRelationship newRelationship)
{
	//If this faction does not exist in the relationship index, return
	if (!factionRelationships.Contains(targetFaction)) return;
	//If the relationship with targetFaction is already set, return
	if (factionRelationships[targetFaction].relationship == newRelationship) return;

	//Set the new relationship and update the target pool to reflect that
	factionRelationships[targetFaction].relationship = newRelationship;
	CleanTargetPool();
	TargetBuildingsOfFaction(targetFaction);
	GetTargetsOfAllies();

	//Perform the same method in the targetFaction object, checks at the top should avoid infinite recursion
	UnitActions::GetFaction(targetFaction)->SetFactionRelationship(faction, newRelationship);
}

void Faction::IncreaseHostility(Factions targetFaction, float& amount)
{
	//Return if not AI-controlled, amount to add <= 0, or the relationship does not exist
	if (!AIControlled || amount <= 0.f) return;
	if (!factionRelationships.Contains(targetFaction)) return;

	//Increase hostility by the amount, clamping between 0 and 1
	float& hostility = factionRelationships[targetFaction].hostilityScale;
	hostility = FMath::Clamp(hostility + amount, 0.f, 1.f);

	//If hostility reaches 1, set relationship to enemy
	if (hostility == 1.f) SetFactionRelationship(targetFaction, FactionRelationship::Enemy);
}

void Faction::LowerHostility(Factions targetFaction, float& amount)
{
	//Return if not AI-controlled, amount to add <= 0, or the relationship does not exist
	if (!AIControlled || amount <= 0.f) return;
	if (!factionRelationships.Contains(targetFaction)) return;

	//Decrease hostility by the amount, clamping between 0 and 1
	float& hostility = factionRelationships[targetFaction].hostilityScale;
	hostility = FMath::Clamp(hostility - amount, 0.f, 1.f);

	//If hostility reaches 0, set relationship to ally
	if (hostility == 0.f) SetFactionRelationship(targetFaction, FactionRelationship::Ally);
}

void Faction::SetFaction(Factions newFaction)
{
	if (newFaction == Factions::None) return;

	faction = newFaction;

	if (newFaction != Factions::Human) AIControlled = true;
}

bool Faction::IsAIControlled()
{
	return AIControlled;
}

void Faction::FeedPop()
{
	if (AIControlled) return;

	int workerAvailableCost = 0;
	int workerFoodCost = 0;

	CalculateFoodCost(workerAvailableCost, workerFoodCost);

	int totalCost = resourceInventory[StratResources::Food].lossesPerDay;

	//Enter full starvation if unaffordable non-working food cost
	if (resourceInventory[StratResources::Food].currentResources < workerAvailableCost)
	{
		StarvePop(totalCost);
		return;
	}
	resourceInventory[StratResources::Food].currentResources -= workerAvailableCost;

	//Enter worker starvation if unaffordable working food cost
	if (resourceInventory[StratResources::Food].currentResources < workerFoodCost)
	{
		StarvePop(totalCost);
		return;
	}
	resourceInventory[StratResources::Food].currentResources -= workerFoodCost;

	currStarveDays = 0;
	starving = false;
}

void Faction::ConsumeEnergy()
{
	if (AIControlled) return;

	int energyCost = resourceInventory[StratResources::Energy].lossesPerDay;

	if (energyCost > resourceInventory[StratResources::Energy].currentResources)
	{
		PowerOutage(energyCost);
		RemoveWorkers(WorkerType::Robot);
		return;
	}

	resourceInventory[StratResources::Energy].currentResources -= energyCost;
	currPowerDays = 0;
	powerOutage = false;
	if (faction == Factions::Human) UnitActions::EnableRobots(Factions::Human, true);
}

void Faction::StarvePop(int foodCost)
{
	int missingFood = foodCost - resourceInventory[StratResources::Food].currentResources;

	resourceInventory[StratResources::Food].currentResources = 0;

	if (!starving)
	{
		starving = true;
	}

	if (currStarveDays < daysTillStarve)
	{
		++currStarveDays;
		return;
	}

	KillPopulation(missingFood, popDeathsPerFoodMissing);
}

void Faction::PowerOutage(int energyCost)
{
	int missingEnergy = energyCost - resourceInventory[StratResources::Energy].currentResources;

	if (!powerOutage)
	{
		powerOutage = true;
		resourceInventory[StratResources::Energy].currentResources = 0;
		if (faction == Factions::Human) UnitActions::EnableRobots(Factions::Human, false);
	}

	if (currPowerDays < daysTillPowerOutage)
	{
		++currPowerDays;
		return;
	}

	if (faction == Factions::Human) UnitActions::EnableRobots(Factions::Human, true);
	KillPopulation(missingEnergy, popDeathsPerPowerMissing);
}

void Faction::RemoveWorkers(WorkerType workerType)
{
	for (ABaseHex* hex : ownedHexes)
	{
		availableWorkers[workerType].available += hex->workersInHex[workerType];
		hex->workersInHex[workerType] = 0;
	}
}

void Faction::KillPopulation(int cost, int deathsPerResource)
{
	int remainingCost = 0;

	//Kill non-working population
	availableWorkers[WorkerType::Human].available -= cost * deathsPerResource;

	if (availableWorkers[WorkerType::Human].available < 0)
	{
		remainingCost = -1 * availableWorkers[WorkerType::Human].available;
		availableWorkers[WorkerType::Human].available = 0;
	}
	else return;

	//Kill working population
	TArray<ABaseHex*> hexesWithWorkers;
	for (ABaseHex* hex : ownedHexes)
	{
		if (hex->workersInHex[WorkerType::Human] > 0) hexesWithWorkers.Add(hex);
	}

	int scanIndex = 0;
	int workersToRemove = FMath::Min(remainingCost, availableWorkers[WorkerType::Human].working);

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
	availableWorkers[WorkerType::Human].working -= remainingCost;

	if (availableWorkers[WorkerType::Human].working < 0)
		availableWorkers[WorkerType::Human].working = 0;
}

void Faction::UpdateResourceCosts()
{
	if (AIControlled) return;

	//Food
	int workerAvailableCost = 0;
	int workerFoodCost = 0;

	CalculateFoodCost(workerAvailableCost, workerFoodCost);
	resourceInventory[StratResources::Food].lossesPerDay = workerAvailableCost + workerFoodCost;

	//Energy
	int energyCost = CalculateEnergyCost();

	resourceInventory[StratResources::Energy].lossesPerDay = energyCost;
}

void Faction::CalculateFoodCost(int& availableWorkerCost, int& workingWorkerCost)
{
	int remainder = availableWorkers[WorkerType::Human].available % foodPerNonWorkers;
	availableWorkerCost = availableWorkers[WorkerType::Human].available / foodPerNonWorkers;
	availableWorkerCost = (availableWorkerCost + (remainder == 0 ? 0 : 1));

	int workerRemainder = 0;

	for (auto& workers : availableWorkers)
	{
		workerRemainder = workers.Value.working % foodPerWorkers;
		int cost = workers.Value.working / foodPerWorkers;
		workingWorkerCost += (cost + (workerRemainder == 0 ? 0 : 1)) * workers.Value.workingFoodCost;
	}
}

int Faction::CalculateEnergyCost()
{
	int energyCost = 0;

	if (!powerOutage)
	{
		for (ATroop* troop : allUnits)
		{
			energyCost += troop->unitStats->energyUpkeep;
		}
	}
	for (ABuilding* building : allBuildings)
	{
		energyCost += building->unitStats->energyUpkeep;
	}
	for (ABaseHex* hex : ownedHexes)
	{
		energyCost += hex->workersInHex[WorkerType::Robot] * UnitActions::GetWorkerEnergyCost(faction)[WorkerType::Robot];
	}

	return energyCost;
}

void Faction::CleanTargetPool()
{
	if (!AIControlled) return;

	TArray<ABaseHex*> targetsToRemove;

	//If targets in the pool are no longer enemies, add to the removal list
	for (auto& target : targetList)
	{
		if (GetFactionRelationship(target.Value) != FactionRelationship::Enemy)
		{
			targetsToRemove.Add(target.Key);
		}
	}

	//Remove the targets from the pool
	for (int i = 0; i < targetsToRemove.Num(); i++)
	{
		targetList.Remove(targetsToRemove[i]);
	}
}

void Faction::GetTargetsOfAllies()
{
	if (!AIControlled) return;

	for (auto& otherFaction : factionRelationships)
	{
		if (otherFaction.Value.relationship != FactionRelationship::Ally) continue;

		for (auto& target : UnitActions::GetFaction(otherFaction.Key)->targetList)
		{
			if (GetFactionRelationship(target.Value) == FactionRelationship::Enemy)
				targetList.Add(target);
		}
	}
}

void Faction::TargetBuildingsOfFaction(Factions targetFaction)
{
	if (!AIControlled || GetFactionRelationship(targetFaction) != FactionRelationship::Enemy) return;

	Faction* factionObject = UnitActions::GetFaction(targetFaction);

	for (auto& building : factionObject->allBuildings)
	{
		if (building->GetOccupier() != Factions::None)
		{
			if (GetFactionRelationship(building->GetOccupier()) == FactionRelationship::Enemy)
			{
				targetList.Add(Cast<ABaseHex>(building->hexNav->currentHex), building->GetOccupier());
				continue;
			}
		}
		else
		{
			targetList.Add(Cast<ABaseHex>(building->hexNav->currentHex), targetFaction);
		}
	}
}
