// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_ResourceCap.h"
#include "FactionController.h"
#include "Faction.h"

float UUAIC_ResourceCap::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return GetMinScore();

	float alpha = 0.f;

	switch (resource)
	{
	case EStratResources::None:
		return GetMinScore();
	case EStratResources::Population:
		alpha = (float)faction->availableWorkers[WorkerType::Alien].available / (float)(faction->availableWorkers[WorkerType::Alien].available + faction->availableWorkers[WorkerType::Alien].working);
		break;
	default:
		alpha = faction->resourceInventory[resource].currentResources / faction->resourceInventory[resource].maxResources;
		break;
	}

	return EvaluateOnCurve(FactorInversion(FMath::Clamp(alpha, 0.f, 1.f)));
}