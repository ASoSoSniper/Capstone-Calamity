// Fill out your copyright notice in the Description page of Project Settings.


#include "MergedArmy.h"

AMergedArmy::AMergedArmy()
{
	unitStats->type = UnitTypes::Army;
	unitStats->HP_current = 0;
	unitStats->HP_max = 0;
	unitStats->defense = 0;
	unitStats->speed = 0;
	unitStats->currentMorale = 0;
	unitStats->maxMorale = 0;
	unitStats->minDamage = 0;
	unitStats->maxDamage = 0;
	unitStats->reinforceRate = 0;
}

void AMergedArmy::ConsumeUnit(ATroop* mergedUnit)
{
	UnitActions::UnitData newData = UnitActions::CollectUnitData(mergedUnit->unitStats);
	
	mergedUnits.Add(newData);

	AddUnitData(newData);

	mergedUnit->Destroy();
}

void AMergedArmy::ConsumeArmy(AMergedArmy* mergedArmy)
{
	for (int i = 0; i < mergedArmy->mergedUnits.Num(); ++i)
	{
		mergedUnits.Add(mergedArmy->mergedUnits[i]);

		AddUnitData(mergedArmy->mergedUnits[i]);
	}

	mergedArmy->Destroy();
}

void AMergedArmy::ConsumeData(TArray<UnitActions::UnitData>& groupData)
{
	unitStats->faction = groupData[0].faction;

	for (int i = 0; i < groupData.Num(); ++i)
	{
		mergedUnits.Add(groupData[i]);

		AddUnitData(groupData[i]);
	}
}

ATroop* AMergedArmy::SpawnUnit(TArray<UnitActions::UnitData>& groupData)
{
	if (groupData.Num() <= 0) return nullptr;

	ATroop* spawnTroop;
	AMergedArmy* spawnArmy;
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);

	hex->troopsInHex.Remove(this);

	bool isArmy = groupData.Num() > 1;
	switch (isArmy)
	{
	case true:
		spawnArmy = spawner->SpawnArmy(Cast<ABaseHex>(hexNav->currentHex), groupData);
		
		return spawnArmy;

	case false:
		spawnTroop = spawner->SpawnTroop(Cast<ABaseHex>(hexNav->currentHex), groupData[0]);

		return spawnTroop;
	}

	return nullptr;
}

void AMergedArmy::AddUnitData(UnitActions::UnitData& unitData)
{
	unitStats->HP_current += unitData.currentHP;
	unitStats->HP_max += unitData.maxHP;
	unitStats->defense += unitData.defense;
	//Something with speed
	unitStats->currentMorale += unitData.currentMorale;
	unitStats->maxMorale += unitData.maxMorale;
	unitStats->minDamage += unitData.minDamage;
	unitStats->maxDamage += unitData.maxDamage;
	unitStats->reinforceRate += unitData.reinforceRate;
}


void AMergedArmy::SplitInHalf()
{
	TArray<UnitActions::UnitData> group1;
	TArray<UnitActions::UnitData> group2;

	//Sort from highest to lowest max damage in each unit
	mergedUnits.Sort([&](const UnitActions::UnitData& A, const UnitActions::UnitData& B)
		{
			return A.maxDamage > B.maxDamage;
		});
	
	//Add even-numbered elements to group 1 and odd-numbered to group 2
	for (int i = 0; i < mergedUnits.Num(); ++i)
	{
		if (i % 2 == 0)
		{
			group1.Add(mergedUnits[i]);
		}
		else
		{
			group2.Add(mergedUnits[i]);
		}
	}

	//Spawn units using the SpawnUnit function, which determines whether that unit will be an army or troop

	SpawnUnit(group1);
	SpawnUnit(group2);	
	this->Destroy();
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
