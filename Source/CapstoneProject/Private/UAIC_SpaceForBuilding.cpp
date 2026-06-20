// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_SpaceForBuilding.h"
#include "Faction.h"

float UUAIC_SpaceForBuilding::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	if (faction->GetPriorityHex_Building(building)) return FactorInversion(1.f);

	return GetMinScore();
}
