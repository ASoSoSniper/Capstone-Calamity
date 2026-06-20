// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_SiegeAdvantage.h"
#include "UnitActions.h"
#include "BattleObject.h"
#include "GlobalSpawner.h"
#include "Building.h"

float UUTC_SiegeAdvantage::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (!hex->building || !UnitActions::IsHostileTarget(
		hex->building->GetUnitData()->GetFaction(), 
		troop->GetUnitData()->GetFaction()))
	{
		return FactorInversion(1.f);
	}

	TMap<UnitTypes, FUnitComposition> attacker = troop->GetUnitData()->GetUnitComposition();

	int attackerDamage = 0;
	
	for (auto& unit : attacker)
	{
		attackerDamage += AGlobalSpawner::spawnerObject->troopStats[unit.Key].siegePower * unit.Value.quantity;
	}

	int defenderDamage = hex->building->GetUnitData()->GetDamage();

	float alpha = static_cast<float>(attackerDamage) / defenderDamage;

	return EvaluateOnCurve(FactorInversion(alpha));
}
