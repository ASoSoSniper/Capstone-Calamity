// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_TroopType.h"

float UUTC_TroopType::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	FUnitData* unitData = troop->GetUnitData();

	if (unitData->GetUnitType() != UnitTypes::Army)
		return FactorInversion(unitData->GetUnitType() == troopType);

	TMap<UnitTypes, FUnitComposition> comp = unitData->GetUnitComposition();
	return FactorInversion(comp.Contains(troopType) && comp[troopType].quantity > 0);
}
