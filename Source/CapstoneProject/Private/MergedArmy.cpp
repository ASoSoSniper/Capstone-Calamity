// Fill out your copyright notice in the Description page of Project Settings.


#include "MergedArmy.h"

AMergedArmy::AMergedArmy()
{
	
}

void AMergedArmy::ConsumeUnit(ATroop* mergedUnit)
{
	FUnitData* newData = mergedUnit->GetUnitData();
	
	unitData->AddUnitData(newData);

	mergedUnit->Destroy();
}

void AMergedArmy::ConsumeData(TArray<FUnitData*>& groupData)
{
	for (int i = 0; i < groupData.Num(); i++)
	{
		unitData->AddUnitData(groupData[i]);
	}
}

ATroop* AMergedArmy::SpawnUnit(TArray<FUnitData*>& groupData)
{
	if (groupData.IsEmpty()) return nullptr;

	float hpPercent = unitData->GetHPAlpha();

	ATroop* spawnTroop;
	AMergedArmy* spawnArmy;
	ABaseHex* hex = hexNav->GetCurrentHex();
	TSet<ABaseHex*> usedHexes;

	hex->troopsInHex.Remove(this);

	bool isArmy = groupData.Num() > 1;
	switch (isArmy)
	{
	case true:
		spawnArmy = AGlobalSpawner::spawnerObject->SpawnArmy(hex->FindFreeAdjacentHex(unitData->GetFaction(), usedHexes), groupData, hpPercent);
		
		return spawnArmy;

	case false:
		spawnTroop = AGlobalSpawner::spawnerObject->SpawnTroop(hex->FindFreeAdjacentHex(unitData->GetFaction(), usedHexes), groupData[0], hpPercent);

		return spawnTroop;
	}

	return nullptr;
}

void AMergedArmy::SplitInHalf()
{
	TArray<FUnitData*> group1;
	TArray<FUnitData*> group2;

	TArray<FUnitData*> units = unitData->GetSavedUnits();

	//Sort from highest to lowest max damage in each unit
	units.Sort([&](const FUnitData& A, const FUnitData& B)
		{
			return A.GetDamage() > B.GetDamage();
		});
	
	//Add even-numbered elements to group 1 and odd-numbered to group 2
	for (int i = 0; i < units.Num(); ++i)
	{
		if (i % 2 == 0)
		{
			group1.Add(units[i]);
		}
		else
		{
			group2.Add(units[i]);
		}
	}

	//Spawn units using the SpawnUnit function, which determines whether that unit will be an army or troop

	SpawnUnit(group1);
	SpawnUnit(group2);	

	unitData->DestroyWorldData();
	this->Destroy();
}

void AMergedArmy::ExtractOneUnit(UnitTypes type)
{
	int32 unitIndex = -1;

	if (unitData->GetSavedUnits().Num() <= 2)
	{
		SplitInHalf();
		return;
	}

	if (!UnitActions::ArmyContainsUnit(this, type, unitIndex)) return;

	ABaseHex* hex = hexNav->GetCurrentHex();
	TSet<ABaseHex*> usedHexes;

	FUnitData* unit = unitData->ExtractUnitData(unitIndex);
	if (!unit) return;

	AGlobalSpawner::spawnerObject->SpawnTroop(hex->FindFreeAdjacentHex(unitData->GetFaction(), usedHexes), unit);
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
