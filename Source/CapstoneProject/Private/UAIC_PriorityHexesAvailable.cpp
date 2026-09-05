// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_PriorityHexesAvailable.h"
#include "Faction.h"

float UUAIC_PriorityHexesAvailable::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	for (const TPair<EStratResources, SpawnableBuildings>& resource : resources)
	{
		if (!faction->GetPriorityHex_Workers(resource.Key)) {
			if (!faction->GetPriorityHex_Building(resource.Value)) return GetMinScore();
		}
	}

	return FactorInversion(1.f);
}
