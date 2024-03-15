// Fill out your copyright notice in the Description page of Project Settings.


#include "SiegeObject.h"
#include "Building.h"

void ASiegeObject::Start()
{
	//Find hex the object is placed on
	hex = Cast<ABaseHex>(hexNav->currentHex);

	building = hex->building;

	//Once created, begin organizing armies into factions
	CreateFactions();
}

void ASiegeObject::Attack()
{
	//End the battle if one group is completely wiped out
	if (currentBattle.Group1.IsEmpty() || (currentBattle.Group2.IsEmpty() && !BuildingIsAlive()))
	{
		EndBattle();
		return;
	}

	CalculateGroupDamage();
	if (BuildingIsAlive()) CalculateSiegeDamage();

	++currentTickTillRoll;
	if (currentTickTillRoll >= ticksTillRoll)
	{
		currentTickTillRoll = 0;
		RollDie(group1Die);
		RollDie(group2Die);
		group1Die = FMath::Clamp(group1Die - hex->defenderBonus, 1, group1Die - hex->defenderBonus);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("Group 1 rolled %d!\nGroup 2 rolled %d!"), group1Die, group2Die));
	}

	int group1DamageTotal = FMath::RoundToInt((float)group1Damage * GetRollModifier(group1Die));
	int group2DamageTotal = FMath::RoundToInt((float)group2Damage * GetRollModifier(group2Die));

	//Apply damage to each group
	for (int i = 0; i < currentBattle.Group1.Num(); ++i)
	{
		armies[currentBattle.Group1[i]].currentHP -= group2DamageTotal;
		armies[currentBattle.Group1[i]].currentHP = FMath::Clamp(armies[currentBattle.Group1[i]].currentHP, 0, armies[currentBattle.Group1[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 1 took %d damage! %d health remaining! %d morale remaining!"), group2DamageTotal, armies[currentBattle.Group1[i]].currentHP, armies[currentBattle.Group1[i]].currentMorale));
	}
	for (int i = 0; i < currentBattle.Group2.Num(); ++i)
	{
		armies[currentBattle.Group2[i]].currentHP -= group1DamageTotal;
		armies[currentBattle.Group2[i]].currentHP = FMath::Clamp(armies[currentBattle.Group2[i]].currentHP, 0, armies[currentBattle.Group2[i]].maxHP);

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Group 2 took %d damage! %d health remaining! %d morale remaining!"), group1DamageTotal, armies[currentBattle.Group2[i]].currentHP, armies[currentBattle.Group1[i]].currentMorale));
	}

	//Apply damage to the building
	building->unitStats->currentHP -= attackerSiegeDamage;
	building->unitStats->currentHP = FMath::Clamp(building->unitStats->currentHP, 0, building->unitStats->maxHP);
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Building took %d damage! %d health remaining!"), attackerSiegeDamage, building->unitStats->currentHP));

	//Remove armies as they are killed
	for (auto army : armies)
	{
		if (!IsAlive(army.Value))
		{
			RemoveArmy(army.Key);
		}
	}
	for (auto army : armies)
	{
		if (DecayMorale(army.Key, moraleDecayRate) <= 0.f)
		{
			FleeFromBattle(army.Key);
		}
	}

	//End the battle if one group is completely wiped out
	if (currentBattle.Group1.IsEmpty() || (currentBattle.Group2.IsEmpty() && !BuildingIsAlive()))
	{
		EndBattle();
	}
}

void ASiegeObject::EndBattle()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Siege ended!")));

	Super::EndBattle();
}

void ASiegeObject::CalculateSiegeDamage()
{
	group2Damage += building->unitStats->damage;

	attackerSiegeDamage = 0;
	for (auto& unit : groupCompositions[0])
	{
		attackerSiegeDamage += spawner->troopStats[unit.Key].siegePower * unit.Value.quantity;
	}
}

bool ASiegeObject::BuildingIsAlive()
{
	building->IsDisabled();

	return !building->disabled;
}
