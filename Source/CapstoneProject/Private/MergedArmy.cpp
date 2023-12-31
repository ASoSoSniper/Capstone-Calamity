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
	
	unitStats->savedUnits.Add(newData);

	AddUnitData(newData);

	mergedUnit->Destroy();
}

void AMergedArmy::ConsumeArmy(AMergedArmy* mergedArmy)
{
	for (int i = 0; i < mergedArmy->unitStats->savedUnits.Num(); ++i)
	{
		unitStats->savedUnits.Add(mergedArmy->unitStats->savedUnits[i]);

		AddUnitData(mergedArmy->unitStats->savedUnits[i]);
	}

	mergedArmy->Destroy();
}

void AMergedArmy::ConsumeData(TArray<UnitActions::UnitData>& groupData)
{
	unitStats->faction = groupData[0].faction;

	for (int i = 0; i < groupData.Num(); ++i)
	{
		unitStats->savedUnits.Add(groupData[i]);

		AddUnitData(groupData[i]);
	}
}

ATroop* AMergedArmy::SpawnUnit(TArray<UnitActions::UnitData>& groupData)
{
	if (groupData.IsEmpty()) return nullptr;

	float currHP = unitStats->HP_current;
	float maxHP = unitStats->HP_max;
	float hpPercent = currHP / maxHP;

	ATroop* spawnTroop;
	AMergedArmy* spawnArmy;
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);

	hex->troopsInHex.Remove(this);

	bool isArmy = groupData.Num() > 1;
	switch (isArmy)
	{
	case true:
		spawnArmy = spawner->SpawnArmy(Cast<ABaseHex>(hexNav->currentHex), groupData, hpPercent);
		
		return spawnArmy;

	case false:
		spawnTroop = spawner->SpawnTroop(Cast<ABaseHex>(hexNav->currentHex), groupData[0], hpPercent);

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
	unitStats->savedUnits.Sort([&](const UnitActions::UnitData& A, const UnitActions::UnitData& B)
		{
			return A.maxDamage > B.maxDamage;
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
