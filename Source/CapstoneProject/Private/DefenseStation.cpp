// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenseStation.h"

ADefenseStation::ADefenseStation()
{
	buildingType = SpawnableBuildings::DefenseStation;
}

void ADefenseStation::BuildingAction()
{
	buff = new FStatusEffect(TEXT("Defense Station Buff"), 
		unitData->GetFaction(), FactionRelationship::Ally, 
		0, moraleBuff, 0, 0, damageBuff, siegeDamageBuff);

	ClaimLand();
}

void ADefenseStation::Destroyed()
{
	if (buff)
	{
		for (ABaseHex* hex : buffedHexes)
		{
			hex->RemoveEffectFromHex(buff);
		}
	}

	Super::Destroyed();
}

void ADefenseStation::ClaimLand()
{
	buffedHexes = hexNav->GetCurrentHex()->GetHexesInRadius(buffRadius);

	for (ABaseHex* hex : buffedHexes)
	{
		hex->AddEffectToHex(buff);
	}
}