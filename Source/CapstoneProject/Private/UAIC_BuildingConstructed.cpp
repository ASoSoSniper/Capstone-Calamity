// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_BuildingConstructed.h"
#include "Faction.h"

float UUAIC_BuildingConstructed::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	const TSet<ABuilding*>& buildings = faction->GetBuildingsOfType(buildingType);

	int completed = 0;
	for (ABuilding* building : buildings)
	{
		if (building->ConstructionComplete())
		{
			completed++;
			if (completed >= count) return FactorInversion(1.f);
		}
	}

	return GetMinScore();
}
