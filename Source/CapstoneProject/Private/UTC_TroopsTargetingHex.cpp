// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_TroopsTargetingHex.h"
#include "Faction.h"

float UUTC_TroopsTargetingHex::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	const TSet<ATroop*>& troops = troop->GetFaction()->GetTroops();
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
