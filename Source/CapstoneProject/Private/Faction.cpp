// Fill out your copyright notice in the Description page of Project Settings.


#include "Faction.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"
#include "TroopFactory.h"


UFaction::UFaction()
{
	faction = Factions::None;
	resourceInventory.Add(EStratResources::Energy, FInventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(EStratResources::Production, FInventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(EStratResources::Food, FInventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(EStratResources::Wealth, FInventoryStat{ 000, 300, 0, 0 });

	availableWorkers.Add(WorkerType::Human, FWorkerStats{ 0,0, 500, 0,1,0 });
	availableWorkers.Add(WorkerType::Robot, FWorkerStats{ 0,0, 0, 1,0,0 });
	availableWorkers.Add(WorkerType::Alien, FWorkerStats{ 0,0, 0, 0,1,0 });

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

	allBuildings.Add(SpawnableBuildings::None, FBuildingSet());
}

void UFaction::FindActiveFactions()
{
	for (auto& currentFaction : ACapstoneProjectGameModeBase::activeFactions)
	{
		if (currentFaction.Key != faction)
		{
			FactionRelationship relationShip = FactionRelationship::Neutral;

			factionRelationships.Add(currentFaction.Key, FRelationshipStats{ currentFaction.Key, relationShip });
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Faction found %d other factions!"), factionRelationships.Num()));
}

void UFaction::SetFoodAndDeathCosts(int foodPerNonWorkersVar, int foodPerWorkersVar, int popDeathsPerFoodMissingVar, int popDeathsPerPowerMissingVar)
{
	foodPerNonWorkers = foodPerNonWorkersVar;
	foodPerWorkers = foodPerWorkersVar;

	popDeathsPerFoodMissing = popDeathsPerFoodMissingVar;
	popDeathsPerPowerMissing = popDeathsPerPowerMissingVar;
}

FactionRelationship UFaction::GetFactionRelationship(Factions targetFaction)
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

FactionRelationship UFaction::GetFactionRelationship(AActor* target)
{
	UUnitStats* stats = target->FindComponentByClass<UUnitStats>();

	if (stats) return GetFactionRelationship(stats->faction);

	return FactionRelationship::Neutral;
}

void UFaction::SetFactionRelationship(Factions targetFaction, FactionRelationship newRelationship)
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

void UFaction::IncreaseHostility(Factions targetFaction, float amount)
{
	//Return if not AI-controlled, amount to add <= 0, or the relationship does not exist
	if (!IsAIControlled() || amount <= 0.f) return;
	if (!factionRelationships.Contains(targetFaction)) return;

	//Increase hostility by the amount, clamping between 0 and 1
	float& hostility = factionRelationships[targetFaction].hostilityScale;
	hostility = FMath::Clamp(hostility + amount, 0.f, 1.f);

	//If hostility reaches 1, set relationship to enemy
	if (hostility == 1.f) SetFactionRelationship(targetFaction, FactionRelationship::Enemy);

	controller->TriggerUpdateDisplay();
}

void UFaction::LowerHostility(Factions targetFaction, float amount)
{
	//Return if not AI-controlled, amount to add <= 0, or the relationship does not exist
	if (!IsAIControlled() || amount <= 0.f) return;
	if (!factionRelationships.Contains(targetFaction)) return;

	//Decrease hostility by the amount, clamping between 0 and 1
	float& hostility = factionRelationships[targetFaction].hostilityScale;
	hostility = FMath::Clamp(hostility - amount, 0.f, 1.f);

	//If hostility reaches 0, set relationship to ally
	if (hostility == 0.f) SetFactionRelationship(targetFaction, FactionRelationship::Ally);

	controller->TriggerUpdateDisplay();
}

void UFaction::SetFaction(Factions newFaction)
{
	if (newFaction == Factions::None) return;

	faction = newFaction;

	if (faction == Factions::Human)
	{
		availableWorkers[WorkerType::Human].available = 100;
		availableWorkers[WorkerType::Robot].available = 10;
	}
	else
	{
		availableWorkers[WorkerType::Alien].available = 1000;
		availableWorkers[WorkerType::Alien].maxAcquired = 1000;
	}
}

Factions UFaction::GetFaction()
{
	return faction;
}

FString UFaction::GetFactionName() const
{
	return factionName;
}

bool UFaction::IsAIControlled()
{
	return faction != Factions::Human;
}

void UFaction::FeedPop()
{
	if (IsAIControlled()) return;

	int workerAvailableCost = 0;
	int workerFoodCost = 0;

	CalculateFoodCost(workerAvailableCost, workerFoodCost);

	int totalCost = resourceInventory[EStratResources::Food].lossesPerDay;

	//Enter full starvation if unaffordable non-working food cost
	if (resourceInventory[EStratResources::Food].currentResources < workerAvailableCost)
	{
		StarvePop(totalCost);
		return;
	}
	resourceInventory[EStratResources::Food].currentResources -= workerAvailableCost;

	//Enter worker starvation if unaffordable working food cost
	if (resourceInventory[EStratResources::Food].currentResources < workerFoodCost)
	{
		StarvePop(totalCost);
		return;
	}
	resourceInventory[EStratResources::Food].currentResources -= workerFoodCost;

	currStarveDays = 0;
	starving = false;
}

void UFaction::ConsumeEnergy()
{
	if (IsAIControlled()) return;

	int energyCost = resourceInventory[EStratResources::Energy].lossesPerDay;

	if (energyCost > resourceInventory[EStratResources::Energy].currentResources)
	{
		PowerOutage(energyCost);
		RemoveWorkers(WorkerType::Robot);
		return;
	}

	resourceInventory[EStratResources::Energy].currentResources -= energyCost;
	currPowerDays = 0;
	powerOutage = false;
	if (faction == Factions::Human) UnitActions::EnableRobots(Factions::Human, true);
}

TArray<ABaseHex*> UFaction::GetHexesOfResource(EStratResources resource, int minValue, bool includeHexesWithBuildings)
{
	TArray<ABaseHex*> hexes = TArray<ABaseHex*>();

	for (ABaseHex* hex : ownedHexes)
	{
		if (!includeHexesWithBuildings && hex->building) continue;
		if (hex->GetNumberOfWorkers() > 0) continue;
		if (!hex->IsBuildableTerrain()) continue;

		FCurrentResourceYields hexYields = hex->GetCurrentResourceYields();
		int yield = 0;

		switch (resource)
		{
		case EStratResources::Food:
			yield = hexYields.foodYield;
			break;
		case EStratResources::Production:
			yield = hexYields.productionYield;
			break;
		case EStratResources::Energy:
			yield = hexYields.energyYield;
			break;
		case EStratResources::Wealth:
			yield = hexYields.wealthYield;
			break;
		}

		if (yield >= minValue) hexes.Add(hex);
	}

	return hexes;
}

const TSet<ABuilding*>& UFaction::GetBuildingsOfType(SpawnableBuildings buildingType) const
{
	if (!allBuildings.Contains(buildingType)) return allBuildings[SpawnableBuildings::None].buildings;

	return allBuildings[buildingType].buildings;
}

void UFaction::AddBuildingToFaction(ABuilding* building)
{
	if (!building) return;

	SpawnableBuildings buildingType = building->GetBuildingType();
	if (!allBuildings.Contains(buildingType)) allBuildings.Add(buildingType, FBuildingSet());

	allBuildings[buildingType].buildings.Add(building);
}

void UFaction::RemoveBuildingFromFaction(ABuilding* building)
{
	if (!building) return;

	SpawnableBuildings buildingType = building->GetBuildingType();
	if (!allBuildings.Contains(buildingType)) return;

	allBuildings[buildingType].buildings.Remove(building);
}

void UFaction::StarvePop(int foodCost)
{
	int missingFood = foodCost - resourceInventory[EStratResources::Food].currentResources;

	resourceInventory[EStratResources::Food].currentResources = 0;

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

void UFaction::PowerOutage(int energyCost)
{
	int missingEnergy = energyCost - resourceInventory[EStratResources::Energy].currentResources;

	if (!powerOutage)
	{
		powerOutage = true;
		resourceInventory[EStratResources::Energy].currentResources = 0;
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

void UFaction::RemoveWorkers(WorkerType workerType)
{
	for (ABaseHex* hex : ownedHexes)
	{
		availableWorkers[workerType].available += hex->workersInHex[workerType];
		hex->workersInHex[workerType] = 0;
	}
}

void UFaction::KillPopulation(int cost, int deathsPerResource)
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

const TArray<FRelationshipStats> UFaction::GetFactionRelationships() const
{
	TArray<FRelationshipStats> relationValues;

	for (const TPair<Factions, FRelationshipStats>& relation : factionRelationships)
	{
		relationValues.Add(relation.Value);
	}

	return relationValues;
}

TMap<EStratResources, int> UFaction::GetAvailableResources() const
{
	TMap<EStratResources, int> resources;

	for (const TPair<EStratResources, FInventoryStat>& resource : resourceInventory)
	{
		resources.Add(resource.Key, resource.Value.currentResources);
	}

	return resources;
}

int UFaction::GetResourceMax() const
{
	return resourceInventory[EStratResources::Food].maxResources;
}

float UFaction::GetPopAlpha() const
{
	const FWorkerStats& stats = availableWorkers[WorkerType::Human];

	return (float)(stats.available + stats.working) / (float)stats.maxAcquired;
}

AFactionController* UFaction::GetFactionController() const
{
	return controller;
}

void UFaction::SetFactionController(AFactionController* setController)
{
	if (controller) return;

	controller = setController;
}

void UFaction::CollectResource(EStratResources resource, int amount)
{
	amount = FMath::Max(amount, 0);

	switch (resource)
	{
	case EStratResources::None:
		return;
	case EStratResources::Population:
		availableWorkers[WorkerType::Human].available += amount;
		availableWorkers[WorkerType::Human].maxAcquired = FMath::Max(availableWorkers[WorkerType::Human].available + availableWorkers[WorkerType::Human].working,
			availableWorkers[WorkerType::Human].maxAcquired);
		break;
	default:
		resourceInventory[resource].currentResources += amount;
		resourceInventory[resource].currentResources = FMath::Clamp(resourceInventory[resource].currentResources, 0, resourceInventory[resource].maxResources);
		break;
	}
}

TMap<EStratResources, int> UFaction::GetNetResourcesPerDay(bool includeIncompleteBuildings) const
{
	using SR = EStratResources;
	TMap<SR, int> resources = {
		{SR::Food, (resourceInventory[SR::Food].resourcePerTick * 6) - resourceInventory[SR::Food].lossesPerDay},
		{SR::Energy, (resourceInventory[SR::Energy].resourcePerTick * 6) - resourceInventory[SR::Energy].lossesPerDay},
		{SR::Production, (resourceInventory[SR::Production].resourcePerTick * 6) - resourceInventory[SR::Production].lossesPerDay},
		{SR::Wealth, (resourceInventory[SR::Wealth].resourcePerTick * 6) - resourceInventory[SR::Wealth].lossesPerDay}
	};

	if (!includeIncompleteBuildings) return resources;

	for (const TPair<SpawnableBuildings, FBuildingSet>& buildingType : allBuildings)
	{
		for (const ABuilding* building : buildingType.Value.buildings)
		{
			if (!building->ConstructionComplete())
			{
				const TMap<SR, int>& yields = building->GetResourceYields();
				for (const TPair<SR, int>& yield : yields)
				{
					resources[yield.Key] += yield.Value;
				}
			}
		}
	}

	return resources;
}

void UFaction::UpdateResourceCosts()
{
	if (IsAIControlled()) return;

	//Food
	int workerAvailableCost = 0;
	int workerFoodCost = 0;

	CalculateFoodCost(workerAvailableCost, workerFoodCost);
	resourceInventory[EStratResources::Food].lossesPerDay = workerAvailableCost + workerFoodCost;

	//Energy
	int energyCost = CalculateEnergyCost();

	resourceInventory[EStratResources::Energy].lossesPerDay = energyCost;
}

FResourceGainLoss UFaction::GetResourceRates()
{
	FResourceGainLoss result;

	TMap<EStratResources, int> resourceGains = UnitActions::GetResourcesPerTick(faction);
	TMap<EStratResources, int> resourceLosses = UnitActions::GetResourceLosses(faction);

	result.wealthGain = resourceGains[EStratResources::Wealth];
	result.wealthLoss = resourceLosses[EStratResources::Wealth];
	result.energyGain = resourceGains[EStratResources::Energy];
	result.energyLoss = resourceLosses[EStratResources::Energy];
	result.foodGain = resourceGains[EStratResources::Food];
	result.foodLoss = resourceLosses[EStratResources::Food];
	result.productionGain = resourceGains[EStratResources::Production];
	result.productionLoss = resourceLosses[EStratResources::Production];

	return result;
}

void UFaction::CalculateFoodCost(int& availableWorkerCost, int& workingWorkerCost)
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

int UFaction::CalculateEnergyCost()
{
	int energyCost = 0;

	if (!powerOutage)
	{
		for (ATroop* troop : allUnits)
		{
			energyCost += troop->GetUnitData()->GetEnergyUpkeep();
		}
	}
	for (const TPair<SpawnableBuildings, FBuildingSet>& buildingType : allBuildings)
	{
		if (buildingType.Value.buildings.IsEmpty()) continue;

		for (ABuilding* building : buildingType.Value.buildings)
		{
			energyCost += building->GetUnitData()->GetEnergyUpkeep();
		}
	}
	for (ABaseHex* hex : ownedHexes)
	{
		energyCost += hex->workersInHex[WorkerType::Robot] * UnitActions::GetWorkerEnergyCost(faction)[WorkerType::Robot];
	}

	return energyCost;
}

void UFaction::CleanTargetPool()
{
	if (!IsAIControlled()) return;

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

void UFaction::GetTargetsOfAllies()
{
	if (!IsAIControlled()) return;

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

void UFaction::TargetBuildingsOfFaction(Factions targetFaction)
{
	if (!IsAIControlled() || GetFactionRelationship(targetFaction) != FactionRelationship::Enemy) return;

	UFaction* factionObject = UnitActions::GetFaction(targetFaction);
	for (const TPair<SpawnableBuildings, FBuildingSet>& buildingType : allBuildings)
	{
		if (buildingType.Value.buildings.IsEmpty()) continue;

		for (ABuilding* building : buildingType.Value.buildings)
		{
			if (building->GetOccupier() != Factions::None)
			{
				if (GetFactionRelationship(building->GetOccupier()) == FactionRelationship::Enemy)
				{
					targetList.Add(building->hexNav->GetCurrentHex(), building->GetOccupier());
					continue;
				}
			}
			else
			{
				targetList.Add(building->hexNav->GetCurrentHex(), targetFaction);
			}
		}
	}
}
