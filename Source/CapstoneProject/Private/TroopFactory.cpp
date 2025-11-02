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
		bool purchased = AGlobalSpawner::spawnerObject->PurchaseTroop(unitData->GetFaction(), unit);
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

float ATroopFactory::GetTroopBuildAlpha() const
{
	UnitTypes unit = GetQueuedTroop();
	if (unit == UnitTypes::None) return 0.f;

	float totalTime = AGlobalSpawner::spawnerObject->troopCosts[unit].timeToBuild;

	return (totalTime - currentTroopBuildTime) / totalTime;
}

UTexture2D* ATroopFactory::GetQueuedTroopIcon() const
{
	UnitTypes unit = GetQueuedTroop();
	if (unit == UnitTypes::None) return nullptr;
	if (!AGlobalSpawner::spawnerObject->troopCosts.Contains(unit)) return nullptr;

	return AGlobalSpawner::spawnerObject->troopCosts[unit].icon;
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

bool ATroopFactory::BuildingTroop() const
{
	return queueSize > 0;
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
	Factions faction = unitData->GetFaction();

	//Build the troop, play a build sound
	UnitTypes unit = DequeueTroop();
	if (unit == UnitTypes::None) return;
	ATroop* troop = AGlobalSpawner::spawnerObject->BuildTroop(faction, unit, hex);
	AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->troopCompleteSound);

	//If the hex has a friendly troop, automatically merge if army cap not exceeded,
	//otherwise place on free adjacent hex
	if (UnitActions::HexHasFriendlyTroop(faction, hex, troop))
	{
		for (int i = 0; i < hex->troopsInHex.Num(); i++)
		{
			if (hex->troopsInHex[i]->GetUnitData()->GetFaction() == faction)
			{
				if (hex->troopsInHex[i]->GetUnitData()->GetSavedUnitCount() < Cast<ATroop>(hex->troopsInHex[i])->GetArmyCap())
				{
					troop->SphereCheck(20.f);
					troop->CommandTroopToMerge(hex->troopsInHex[i]);
				}
				else
				{
					TSet<ABaseHex*> usedHexes;
					ABaseHex* freeHex = hex->FindFreeAdjacentHex(faction, usedHexes);

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
