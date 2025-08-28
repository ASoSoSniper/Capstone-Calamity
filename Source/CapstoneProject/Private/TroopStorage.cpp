// Fill out your copyright notice in the Description page of Project Settings.


#include "TroopStorage.h"
#include "Troop.h"
#include "MergedArmy.h"
#include "CapstoneProjectGameModeBase.h"

ATroopStorage::ATroopStorage()
{
	buildingType = SpawnableBuildings::RobotBarracks;
}

void ATroopStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ConstructionComplete()) return;

	float scaledTime = DeltaTime * ACapstoneProjectGameModeBase::timeScale;

	HealTroops(DeltaTime);
}

void ATroopStorage::StoreTroop(ATroop* troop)
{
	UnitActions::UnitData troopData = UnitActions::CollectUnitData(troop->unitStats);
	troopsInStorage.Add(troopData);

	troop->Destroy();
}

TArray<ATroop*> ATroopStorage::ReleaseTroops()
{
	TArray<ATroop*> spawnedTroops;
	TSet<ABaseHex*> usedHexes;
	ABaseHex* hex = hexNav->GetCurrentHex();

	for (int i = 0; i < troopsInStorage.Num(); ++i)
	{
		ATroop* spawn = nullptr;
		ABaseHex* spawnPoint = hex->FindFreeAdjacentHex(hex->GetHexOwner(), usedHexes);
		usedHexes.Add(spawnPoint);

		switch (troopsInStorage[i].unitType)
		{
		case UnitTypes::Army:
			spawn = AGlobalSpawner::spawnerObject->SpawnArmy(spawnPoint, troopsInStorage[i].savedUnits);
			break;

		default:
			spawn = AGlobalSpawner::spawnerObject->SpawnTroop(spawnPoint, troopsInStorage[i]);
			break;
		}

		spawnedTroops.Add(spawn);
	}
	troopsInStorage.Empty();
	return spawnedTroops;
}

void ATroopStorage::HealTroops(float& DeltaTime)
{
	if (currentHealRate > 0)
	{
		currentHealRate -= DeltaTime;
		return;
	}

	currentHealRate = healRate;
	ABaseHex* hex = hexNav->GetCurrentHex();

	if (hex->troopsInHex.IsEmpty()) return;

	for (int i = 0; i < hex->troopsInHex.Num(); i++)
	{
		hex->troopsInHex[i]->unitStats->Heal(healAmount);
	}
}