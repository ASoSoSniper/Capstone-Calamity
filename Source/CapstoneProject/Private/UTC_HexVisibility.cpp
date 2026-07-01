// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_HexVisibility.h"

float UUTC_HexVisibility::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	EFactions faction = troop->GetUnitData()->GetFaction();

	float visible = hex->visibility->VisibleToFaction(faction) ? 0.5f : 0.f;
	float discovered = hex->visibility->DiscoveredByFaction(faction) ? 0.5f : 0.f;

	return EvaluateOnCurve(FactorInversion(visible + discovered));
}
