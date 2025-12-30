// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_BuildingCount.h"
#include "FactionController.h"
#include "Faction.h"

float UUAIC_BuildingCount::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return GetMinScore();

	const TSet<ABuilding*>& buildings = faction->GetBuildingsOfType(buildingType);

	if (value == 0 && buildings.IsEmpty()) return FactorInversion(1.f);

	int count = buildings.Num();
	if (greaterEqual)
		return FactorInversion(count >= value);
	else
		return FactorInversion(count <= value);
}
