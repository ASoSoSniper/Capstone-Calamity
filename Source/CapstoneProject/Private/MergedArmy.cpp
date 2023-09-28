// Fill out your copyright notice in the Description page of Project Settings.


#include "MergedArmy.h"

AMergedArmy::AMergedArmy()
{
	unitStats->HP_current = 0;
	unitStats->HP_max = 0;
	unitStats->defense = 0;
	unitStats->speed = 0;
	unitStats->minDamage = 0;
	unitStats->maxDamage = 0;
}

void AMergedArmy::ConsumeUnit(ATroop* mergedUnit)
{
	UnitActions::UnitData newData = UnitActions::CollectUnitData(mergedUnit->unitStats);
	
	mergedUnits.Add(newData);

	unitStats->HP_current += newData.currentHP;
	unitStats->HP_max += newData.maxHP;
	unitStats->defense += newData.defense;
	//Something with speed
	unitStats->minDamage += newData.minDamage;
	unitStats->maxDamage += newData.maxDamage;
	//Still need morale
	//Still need regen rate

	mergedUnit->Destroy();
}

void AMergedArmy::ConsumeArmy(AMergedArmy* mergedArmy)
{
	for (int i = 0; i < mergedArmy->mergedUnits.Num(); ++i)
	{
		mergedUnits.Add(mergedArmy->mergedUnits[i]);

		unitStats->HP_current += mergedArmy->mergedUnits[i].currentHP;
		unitStats->HP_max += mergedArmy->mergedUnits[i].maxHP;
		unitStats->defense += mergedArmy->mergedUnits[i].defense;
		//Something with speed
		unitStats->minDamage += mergedArmy->mergedUnits[i].minDamage;
		unitStats->maxDamage += mergedArmy->mergedUnits[i].maxDamage;
	}

	mergedArmy->Destroy();
}

void AMergedArmy::ConsumeData(TArray<UnitActions::UnitData>& groupData)
{
	unitStats->faction = groupData[0].faction;

	for (int i = 0; i < groupData.Num(); ++i)
	{
		mergedUnits.Add(groupData[i]);

		unitStats->HP_current += groupData[i].currentHP;
		unitStats->HP_max += groupData[i].maxHP;
		unitStats->defense += groupData[i].defense;
		//Something with speed
		unitStats->minDamage += groupData[i].minDamage;
		unitStats->maxDamage += groupData[i].maxDamage;
	}
}

ATroop* AMergedArmy::SpawnUnit(TArray<UnitActions::UnitData>& groupData)
{
	if (groupData.Num() <= 0) return nullptr;

	ATroop* spawnTroop;
	AMergedArmy* spawnArmy;
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
	FActorSpawnParameters params;

	hex->troopsInHex.Remove(this);

	bool isArmy = groupData.Num() > 1;
	switch (isArmy)
	{
	case true:
		spawnArmy = GetWorld()->SpawnActor<AMergedArmy>(mergedArmyPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
		hex->troopsInHex.Add(spawnArmy);

		spawnArmy->ConsumeData(groupData);
		
		return spawnArmy;

	case false:
		spawnTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
		hex->troopsInHex.Add(spawnTroop);	

		spawnTroop->InputUnitStats(groupData[0]);

		return spawnTroop;
	}

	return nullptr;
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
