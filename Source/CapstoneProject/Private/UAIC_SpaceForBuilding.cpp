// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_SpaceForBuilding.h"
#include "FactionController.h"
#include "Faction.h"

float UUAIC_SpaceForBuilding::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return GetMinScore();

	if (faction->GetPriorityHex_Building(building)) return FactorInversion(1.f);

	return GetMinScore();
}
