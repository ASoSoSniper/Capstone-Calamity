// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_BuildingConstructed.h"
#include "FactionController.h"
#include "Faction.h"

float UUAIC_BuildingConstructed::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
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
