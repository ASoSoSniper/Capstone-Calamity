// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_BattleAdvantage.h"
#include "UnitActions.h"
#include "BattleObject.h"

float UUTC_BattleAdvantage::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (UnitActions::FindHostileTarget(troop->GetUnitData()->GetFaction(), hex) == EFactions::None)
		return FactorInversion(1.f);

	TMap<UnitTypes, FUnitComposition> attacker = troop->GetUnitData()->GetUnitComposition();
	EFactions faction = troop->GetUnitData()->GetFaction();

	TMap<UnitTypes, FUnitComposition> defender;

	TArray<AMovementAI*> troopsInHex = hex->troopsInHex;
	if (!troopsInHex.IsEmpty())
	{
		int totalUnits = 0;
		for (AMovementAI* troop : troopsInHex)
		{
			FUnitData* unitData = troop->GetUnitData();
			if (UnitActions::IsHostileTarget(faction, unitData->GetFaction()))
			{
				TMap<UnitTypes, FUnitComposition> composition = unitData->GetUnitComposition();
				for (const TPair<UnitTypes, FUnitComposition>& unit : composition)
				{
					if (!defender.Contains(unit.Key))
						defender.Add(unit.Key, FUnitComposition());

					defender[unit.Key].quantity += unit.Value.quantity;
					totalUnits += unit.Value.quantity;
				}
			}
		}

		for (auto& unit : defender)
		{
			unit.Value.compPercent = static_cast<float>(unit.Value.quantity) / totalUnits;
		}
	}

	int attackerDamage = ABattleObject::GetGroupDamage(attacker, defender);
	int defenderDamage = ABattleObject::GetGroupDamage(defender, attacker);

	float alpha = static_cast<float>(attackerDamage) / defenderDamage;

	return EvaluateOnCurve(FactorInversion(alpha));
}