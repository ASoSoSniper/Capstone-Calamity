// Fill out your copyright notice in the Description page of Project Settings.


#include "MergedArmy.h"

AMergedArmy::AMergedArmy()
{
	unitStats->unitType = UnitTypes::Army;
	unitStats->currentHP = 0;
	unitStats->maxHP = 0;
	unitStats->currentMorale = 0;
	unitStats->maxMorale = 0;
	
	unitStats->vision = 0;
	unitStats->speed = 0;

	unitStats->damage = 0;
	unitStats->siegePower = 0;

	unitStats->reinforceRate = 0;
	unitStats->energyUpkeep = 0;
}

void AMergedArmy::ConsumeUnit(ATroop* mergedUnit)
{
	UnitActions::UnitData newData = UnitActions::CollectUnitData(mergedUnit->unitStats);
	
	unitStats->savedUnits.Add(newData);

	UnitActions::AddUnitData(unitStats, newData);

	mergedUnit->Destroy();
}

void AMergedArmy::ConsumeArmy(AMergedArmy* mergedArmy)
{
	for (int i = 0; i < mergedArmy->unitStats->savedUnits.Num(); ++i)
	{
		unitStats->savedUnits.Add(mergedArmy->unitStats->savedUnits[i]);

		UnitActions::AddUnitData(unitStats, mergedArmy->unitStats->savedUnits[i]);
	}

	mergedArmy->Destroy();
}

void AMergedArmy::ConsumeData(TArray<UnitActions::UnitData>& groupData)
{
	for (int i = 0; i < groupData.Num(); ++i)
	{
		unitStats->savedUnits.Add(groupData[i]);

		UnitActions::AddUnitData(unitStats, groupData[i]);
	}
}

ATroop* AMergedArmy::SpawnUnit(TArray<UnitActions::UnitData>& groupData)
{
	if (groupData.IsEmpty()) return nullptr;

	float currHP = unitStats->currentHP;
	float maxHP = unitStats->maxHP;
	float hpPercent = currHP / maxHP;

	ATroop* spawnTroop;
	AMergedArmy* spawnArmy;
	ABaseHex* hex = hexNav->GetCurrentHex();
	TArray<ABaseHex*> ignoredHexes;

	hex->troopsInHex.Remove(this);

	bool isArmy = groupData.Num() > 1;
	switch (isArmy)
	{
	case true:
		spawnArmy = AGlobalSpawner::spawnerObject->SpawnArmy(hex->FindFreeAdjacentHex(unitStats->faction, ignoredHexes), groupData, hpPercent);
		
		return spawnArmy;

	case false:
		spawnTroop = AGlobalSpawner::spawnerObject->SpawnTroop(hex->FindFreeAdjacentHex(unitStats->faction, ignoredHexes), groupData[0], hpPercent);

		return spawnTroop;
	}

	return nullptr;
}

void AMergedArmy::SplitInHalf()
{
	TArray<UnitActions::UnitData> group1;
	TArray<UnitActions::UnitData> group2;

	//Sort from highest to lowest max damage in each unit
	unitStats->savedUnits.Sort([&](const UnitActions::UnitData& A, const UnitActions::UnitData& B)
		{
			return A.damage > B.damage;
		});
	
	//Add even-numbered elements to group 1 and odd-numbered to group 2
	for (int i = 0; i < unitStats->savedUnits.Num(); ++i)
	{
		if (i % 2 == 0)
		{
			group1.Add(unitStats->savedUnits[i]);
		}
		else
		{
			group2.Add(unitStats->savedUnits[i]);
		}
	}

	//Spawn units using the SpawnUnit function, which determines whether that unit will be an army or troop

	SpawnUnit(group1);
	SpawnUnit(group2);	
	this->Destroy();
}

void AMergedArmy::ExtractOneUnit(UnitTypes type)
{
	int32 unitIndex = -1;

	if (unitStats->savedUnits.Num() <= 2)
	{
		SplitInHalf();
		return;
	}

	if (!UnitActions::ArmyContainsUnit(this, type, unitIndex)) return;

	ABaseHex* hex = hexNav->GetCurrentHex();
	TArray<ABaseHex*> ignoredHexes;

	UnitActions::UnitData unit = UnitActions::ExtractUnit(unitStats, unitIndex);
	if (unit.unitType == UnitTypes::None) return;

	AGlobalSpawner::spawnerObject->SpawnTroop(hex->FindFreeAdjacentHex(unitStats->faction, ignoredHexes), unit);
}

void AMergedArmy::Action1()
{

}

void AMergedArmy::Action3()
{
	SplitInHalf();
}

void AMergedArmy::Action4()
{
}
