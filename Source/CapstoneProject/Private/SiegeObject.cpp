// Fill out your copyright notice in the Description page of Project Settings.


#include "SiegeObject.h"
#include "Building.h"
#include "BasePlayerController.h"

void ASiegeObject::Start()
{
	if (ABaseHex* centerHex = hexNav->GetCurrentHex())
	{
		building = centerHex->building;
		if (building)
		{
			TSet<ABaseHex*> surroundingHexes = centerHex->GetHexesInRadius(building->GetHexLayersToOccupy());
			for (ABaseHex* adjHex : surroundingHexes)
			{
				adjHex->battle = this;
			}
		}
	}

	Super::Start();
}

ABuilding* ASiegeObject::GetBuilding()
{
	return building;
}

void ASiegeObject::CalculateGroupDamage(int& group1Damage, int& group2Damage)
{
	Super::CalculateGroupDamage(group1Damage, group2Damage);

	if (!BuildingIsAlive()) return;

	group2Damage += building->GetUnitData()->GetDamage();

	group1SiegeDamage = 0;
	for (auto& unit : groupCompositions[0])
	{
		group1SiegeDamage += AGlobalSpawner::spawnerObject->troopStats[unit.Key].siegePower * unit.Value.quantity;
	}
}
void ASiegeObject::ApplyGroupDamage(const int& group1Damage, const int& group2Damage)
{
	Super::ApplyGroupDamage(group1Damage, group2Damage);

	building->GetUnitData()->DamageHP(group1SiegeDamage);

	if (EndCondition()) EndBattle();
}

void ASiegeObject::GenerateModels()
{
	USkeletalMesh* robotMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));
	USkeletalMesh* alienMesh = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));

	if (currentBattle.Group1.Contains(Factions::Human))
	{
		group1Mesh->SetSkeletalMesh(robotMesh);
		group1Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group1Mesh->SetSkeletalMesh(alienMesh);
		group1Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}

	if (building->GetUnitData()->GetFaction() == Factions::Human || building->GetOccupier() == Factions::Human)
	{
		group2Mesh->SetSkeletalMesh(robotMesh);
		group2Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Human].visibleTexture);
	}
	else
	{
		group2Mesh->SetSkeletalMesh(alienMesh);
		group2Mesh->SetMaterial(0, AGlobalSpawner::spawnerObject->troopFactionMaterials[Factions::Alien1].visibleTexture);
	}
}

void ASiegeObject::DestroyBattle()
{
	if (building->GetUnitData()->GetFaction() == Factions::Human && !BuildingIsAlive()) building->Destroy();

	TSet<ABaseHex*> surroundingHexes = GetHex()->GetHexesInRadius(building->GetHexLayersToOccupy());
	for (ABaseHex* adjHex : surroundingHexes)
	{
		adjHex->battle = nullptr;
	}

	Super::DestroyBattle();
}

bool ASiegeObject::BuildingIsAlive() const
{
	bool disabled = building->IsDisabled();
	if (disabled)
	{
		if (!currentBattle.Group1.IsEmpty())
		{
			building->SetSiegeState(true, attackingFaction);
		}
	}

	return !building->IsOccupied();
}
bool ASiegeObject::EndCondition() const
{
	return currentBattle.Group1.IsEmpty() || (currentBattle.Group2.IsEmpty() && !BuildingIsAlive());
}
bool ASiegeObject::Group2IsAlive(bool& containsHuman) const
{
	bool humanTroops = false;
	bool humanBuilding = false;

	bool troopsAlive = Super::Group2IsAlive(humanTroops);
	bool buildingAlive = building->GetUnitData()->IsAlive();

	containsHuman = humanTroops || humanBuilding;

	return troopsAlive || buildingAlive;
}
