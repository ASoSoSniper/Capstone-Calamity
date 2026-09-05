// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_ResourceCosts.h"
#include "Faction.h"

float UUAIC_ResourceCosts::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	int gains = faction->GetResourceGainsPerDay(resource, includeIncompleteBuildings);
	int losses = faction->GetResourceLossesPerDay(resource);

	if (losses == 0) return GetMinScore();

	float alpha = static_cast<float>(gains) / losses;

	return EvaluateOnCurve(FactorInversion(1 - FMath::Min(alpha, targetRatio) / targetRatio));
}
