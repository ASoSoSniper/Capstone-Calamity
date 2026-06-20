// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_EnemyOnHex.h"
#include "Faction.h"
#include "UnitActions.h"

float UUTC_EnemyOnHex::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (hex->troopsInHex.IsEmpty() || !hex->building)
		return FactorInversion(0.f);

	UFaction* troopFaction = troop->GetFaction();
	EFactions buildingFaction = hex->building->GetUnitData()->GetFaction();

	bool enemyTroops = UnitActions::FindHostileTarget(troopFaction->GetFaction(), hex) != EFactions::None;
	bool enemyBuilding = troopFaction->GetFactionRelationship(buildingFaction) == EFactionRelationship::Enemy;

	return FactorInversion(enemyTroops || enemyBuilding);
}
