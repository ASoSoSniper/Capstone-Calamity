// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_HexesFree.h"
#include "Faction.h"

float UUAIC_HexesFree::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	float occupied = faction->GetOccupiedHexCount();
	float total = faction->GetOwnedHexCount();

	float alpha = FMath::Clamp(occupied / total, 0.f, 0.1f);
	return EvaluateOnCurve(FactorInversion(alpha));
}
