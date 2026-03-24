// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_HexesFree.h"
#include "Faction.h"
#include "FactionController.h"

float UUAIC_HexesFree::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return GetMinScore();

	float occupied = faction->GetOccupiedHexCount();
	float total = faction->GetOwnedHexCount();

	float alpha = FMath::Clamp(occupied / total, 0.f, 0.1f);
	return EvaluateOnCurve(FactorInversion(alpha));
}
