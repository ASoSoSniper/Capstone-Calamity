// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_HexVisibility.h"

float UUTC_HexVisibility::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	UFaction* faction = troop->GetFaction();
	if (!faction) return GetMinScore();
	
	return ScoreCondition(faction, hex);
}

float UUTC_HexVisibility::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	EFactions factionEnum = faction->GetFaction();
	float visible = hex->visibility->VisibleToFaction(factionEnum) ? 0.5f : 0.f;
	float discovered = hex->visibility->DiscoveredByFaction(factionEnum) ? 0.5f : 0.f;

	return EvaluateOnCurve(FactorInversion(visible + discovered));
}
