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

	//Created array of armies to remove this combat tick
	TArray<Factions> armiesToRemove;

	//Add armies to the remove list as they are killed or their morale is depleted
	for (auto& army : armies)
	{
		if (!IsAlive(army.Value))
		{
			armiesToRemove.Add(army.Key);
			continue;
		}

		if (DecayMorale(army.Key, moraleDecayRate) <= 0.f)
		{
			armiesToRemove.Add(FleeFromBattle(army.Key));
		}
	}

	//Remove armies that satisfy removal requirements
	if (!armiesToRemove.IsEmpty())
	{
		for (int i = 0; i < armiesToRemove.Num(); i++)
		{
			RemoveArmy(armiesToRemove[i]);
		}
	}

	//End the battle if Group 1 is completely wiped out or Group 2 AND its building are wiped out
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

void ASiegeObject::GenerateModels()
{
	USkeletalMesh* robotMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));
	USkeletalMesh* alienMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));

	if (currentBattle.Group1.Contains(Factions::Human))
	{
		group1Mesh->SetSkeletalMesh(robotMesh);
		group1Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group1Mesh->SetSkeletalMesh(alienMesh);
		group1Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}

	if (building->unitStats->faction == Factions::Human)
	{
		group2Mesh->SetSkeletalMesh(robotMesh);
		group2Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group2Mesh->SetSkeletalMesh(alienMesh);
		group2Mesh->SetMaterial(0, spawner->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}

	UAnimBlueprint* animBP = LoadObject<UAnimBlueprint>(nullptr, TEXT("/Game/3DModels/Animations/Robot_Beta_Combat_BP.Robot_Beta_Combat_BP"));
	if (animBP)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Combat BP Found!"));

		group1Mesh->SetAnimInstanceClass(animBP->GeneratedClass);
		group2Mesh->SetAnimInstanceClass(animBP->GeneratedClass);
	}
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
	bool alive = building->IsDisabled();
	if (alive)
	{
		building->siegingFaction = currentBattle.Group1[0];
	}

	return !building->sieged;
}
