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
	FUnitData* troopData = troop->GetUnitData();
	troopsInStorage.Add(troopData);

	troop->Destroy();

	SetUpdateUI();
}

TArray<ATroop*> ATroopStorage::ReleaseAllTroops()
{
	TArray<ATroop*> spawnedTroops;
	TSet<ABaseHex*> usedHexes;
	ABaseHex* hex = hexNav->GetCurrentHex();

	for (int i = 0; i < troopsInStorage.Num(); ++i)
	{
		ATroop* spawn = ReleaseTroop(i, hex, usedHexes);
		spawnedTroops.Add(spawn);
	}
	troopsInStorage.Empty();
	return spawnedTroops;
}

ATroop* ATroopStorage::ReleaseOneTroop(int index)
{
	TSet<ABaseHex*> usedHexes;
	ATroop* spawn = ReleaseTroop(index, hexNav->GetCurrentHex(), usedHexes);
	troopsInStorage.RemoveAt(index);

	return spawn;
}

ATroop* ATroopStorage::ReleaseTroop(int index, ABaseHex* hex, TSet<ABaseHex*>& usedHexes)
{
	ABaseHex* spawnPoint = hex->FindFreeAdjacentHex(hex->GetHexOwner(), usedHexes);
	ATroop* spawn = AGlobalSpawner::spawnerObject->SpawnArmyByUnit(spawnPoint, troopsInStorage[index]);

	if (spawn) SetUpdateUI();

	return spawn;
}

const FUnitData* ATroopStorage::GetStoredTroopInfo(int index) const
{
	if (!troopsInStorage.IsValidIndex(index)) return nullptr;

	return troopsInStorage[index];
}

TArray<FUnitData*> ATroopStorage::GetAllStoredTroops() const
{
	return troopsInStorage;
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
		hex->troopsInHex[i]->GetUnitData()->HealHP(healAmount);
	}

	SetUpdateUI();
}

void ATroopStorage::SetUpdateUI()
{
	updateUI = true;
}

bool ATroopStorage::UpdateUI()
{
	if (updateUI)
	{
		updateUI = false;
		return true;
	}

	return false;
}