// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_TroopsTargetingHex.h"
float UUTC_TroopsTargetingHex::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (!troop || !hex) return GetMinScore();

	UFaction* faction = troop->GetFaction();
	if (!faction) return GetMinScore();

	return ScoreCondition(faction, hex);
}

float UUTC_TroopsTargetingHex::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	if (!faction || !hex) return GetMinScore();

	const TSet<ATroop*>& troops = faction->GetTroops();
	if (troops.IsEmpty()) return FactorInversion(1.f);

	int troopsTargeting = 0;

	for (ATroop* t : troops)
	{
		if (t->hexNav->GetTargetHex() == hex) {
			troopsTargeting++;
			if (troopsTargeting >= maxTroops) break;
		}
	}

	float alpha = (maxTroops - (float)troopsTargeting) / maxTroops;

	return EvaluateOnCurve(FactorInversion(alpha));
}
