// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_Building_QueueTroops.h"
#include "Outpost.h"
#include "GlobalSpawner.h"

void UUAI_Building_QueueTroops::ExecuteAction(IUAI_Controller* controller)
{
	if (troopsToBuild.IsEmpty()) return;

	using Unit = TPair<UnitTypes, int>;

	AOutpost* outpost = Cast<AOutpost>(controller);
	if (!outpost) return;

	TArray<Unit> orderedQueue;
	int totalUnits = 0;
	for (Unit& unitType : troopsToBuild)
	{
		orderedQueue.Add(Unit(unitType.Key, 0));
		totalUnits += unitType.Value;
	}

	orderedQueue.Sort([&](const Unit& a, const Unit& b) { return SortTroopTypes(a.Key, b.Key); });

	int currIndex = 0;
	int queuesInCycle = 0;

	while (totalUnits > 0)
	{
		if (orderedQueue[currIndex].Value < troopsToBuild[orderedQueue[currIndex].Key])
		{
			outpost->QueueTroopBuild(orderedQueue[currIndex].Key);
			orderedQueue[currIndex].Value++;
			totalUnits--;
			queuesInCycle++;
		}

		bool maxTroopsQueued = orderedQueue[currIndex].Value >= troopsToBuild[orderedQueue[currIndex].Key];
		bool maxQueuesInCycle = cycleTroopsQueued ? (queuesInCycle >= cycleTroopAtCount) : false;
		
		if (maxTroopsQueued || maxQueuesInCycle)
		{
			currIndex = ((currIndex + 1) <= (orderedQueue.Num() - 1)) ? (currIndex + 1) : 0;
			queuesInCycle = 0;
		}
	}
}

bool UUAI_Building_QueueTroops::SortTroopTypes(const UnitTypes& a, const UnitTypes& b)
{
	bool value = false;

	switch (buildOrder)
	{
	case EBuildOrder::Random:
		value = 
			FMath::RandRange(0.f, 1.f) > 0.5f;
		break;
	case EBuildOrder::Price:
		value = 
			AGlobalSpawner::spawnerObject->troopCosts[a].productionCost < 
			AGlobalSpawner::spawnerObject->troopCosts[b].productionCost;
		break;
	case EBuildOrder::BuildSpeed:
		value =
			AGlobalSpawner::spawnerObject->troopCosts[a].timeToBuild <
			AGlobalSpawner::spawnerObject->troopCosts[b].timeToBuild;
		break;
	case EBuildOrder::Strength:
		value =
			AGlobalSpawner::spawnerObject->troopStats[a].damage <
			AGlobalSpawner::spawnerObject->troopStats[b].damage;
		break;
	}

	return ascendingOrder ? !value : value;
}
