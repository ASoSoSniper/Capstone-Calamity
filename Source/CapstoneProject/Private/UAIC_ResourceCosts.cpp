// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_ResourceCosts.h"
#include "Faction.h"

float UUAIC_ResourceCosts::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	float gains = faction->GetResourceGainsPerDay(resource, includeIncompleteBuildings);
	float losses = faction->GetResourceLossesPerDay(resource);

	float alpha = 1.f - FMath::Clamp(FMath::Max(gains, 1.f) / FMath::Max(losses, 1.f), 0.f, 1.f);

	return EvaluateOnCurve(FactorInversion(alpha));
}
