// Fill out your copyright notice in the Description page of Project Settings.


#include "TroopFactory.h"
#include "CapstoneProjectGameModeBase.h"

ATroopFactory::ATroopFactory()
{
	buildingType = SpawnableBuildings::RobotFactory;
}

void ATroopFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ConstructionComplete()) return;

	float scaledTime = DeltaTime * ACapstoneProjectGameModeBase::timeScale;

	BuildTroop(scaledTime);
}

void ATroopFactory::QueueTroopBuild(UnitTypes unit)
{
	if (!AGlobalSpawner::spawnerObject) return;

	if (AGlobalSpawner::spawnerObject->troopCosts.Contains(unit))
	{
		bool purchased = AGlobalSpawner::spawnerObject->PurchaseTroop(unitStats->faction, unit);
		if (purchased)
		{
			EnqueueTroop(unit);
			if (queueSize == 1) currentTroopBuildTime = AGlobalSpawner::spawnerObject->troopCosts[unit].timeToBuild;
		}
	}
}

float ATroopFactory::GetTroopBuildTime() const
{
	return currentTroopBuildTime;
}

UnitTypes ATroopFactory::GetQueuedTroop() const
{
	UnitTypes unit = UnitTypes::None;
	queuedUnits.Peek(unit);

	return unit;
}

int ATroopFactory::GetQueueSize() const
{
	return queueSize;
}

void ATroopFactory::BuildTroop(const float& DeltaTime)
{
	if (currentTroopBuildTime > 0)
	{
		currentTroopBuildTime -= DeltaTime;
	}
	else
	{
		if (!queuedUnits.IsEmpty() && AGlobalSpawner::spawnerObject)
		{
			SpawnTroop();

			if (!queuedUnits.IsEmpty())
			{
				currentTroopBuildTime = AGlobalSpawner::spawnerObject->troopCosts[GetQueuedTroop()].timeToBuild;
			}
		}
	}
}

void ATroopFactory::SpawnTroop()
{
	ABaseHex* hex = hexNav->GetCurrentHex();

	//Build the troop, play a build sound
	UnitTypes unit = DequeueTroop();
	if (unit == UnitTypes::None) return;
	ATroop* troop = AGlobalSpawner::spawnerObject->BuildTroop(unitStats->faction, unit, hex);
	AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->troopCompleteSound);

	//If the hex has a friendly troop, automatically merge if army cap not exceeded,
	//otherwise place on free adjacent hex
	if (UnitActions::HexHasFriendlyTroop(unitStats->faction, hex, troop))
	{
		for (int i = 0; i < hex->troopsInHex.Num(); i++)
		{
			if (hex->troopsInHex[i]->unitStats->faction == unitStats->faction)
			{
				if (hex->troopsInHex[i]->unitStats->savedUnits.Num() < Cast<ATroop>(hex->troopsInHex[i])->GetArmyCap())
				{
					troop->SphereCheck(20.f);
					troop->CommandTroopToMerge(hex->troopsInHex[i]);
				}
				else
				{
					TSet<ABaseHex*> usedHexes;
					ABaseHex* freeHex = hex->FindFreeAdjacentHex(unitStats->faction, usedHexes);

					troop->SetActorLocation(freeHex->troopAnchor->GetComponentLocation());
				}

				break;
			}
		}
	}
}

void ATroopFactory::EnqueueTroop(UnitTypes unit)
{
	if (unit == UnitTypes::None) return;

	queuedUnits.Enqueue(unit);
	queueSize++;
}

UnitTypes ATroopFactory::DequeueTroop()
{
	UnitTypes unit = UnitTypes::None;
	queuedUnits.Dequeue(unit);

	if (unit != UnitTypes::None)
		queueSize = FMath::Max(queueSize - 1, 0);

	return unit;
}
