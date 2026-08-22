// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_EnemyOnHex.h"
#include "UnitActions.h"

float UUTC_EnemyOnHex::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (hex->troopsInHex.IsEmpty() || !hex->building)
		return GetMinScore();

	UFaction* troopFaction = troop->GetFaction();
	return ScoreCondition(troopFaction, hex);
}

float UUTC_EnemyOnHex::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	EFactions buildingFaction = hex->building->GetUnitData()->GetFaction();

	bool enemyTroops = UnitActions::FindHostileTarget(faction->GetFaction(), hex) != EFactions::None;
	bool enemyBuilding = faction->GetFactionRelationship(buildingFaction) == EFactionRelationship::Enemy;

	return FactorInversion(enemyTroops || enemyBuilding);
}
