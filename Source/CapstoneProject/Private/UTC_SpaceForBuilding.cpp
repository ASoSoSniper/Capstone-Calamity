// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_SpaceForBuilding.h"
#include "GlobalSpawner.h"

float UUTC_SpaceForBuilding::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	if (!AGlobalSpawner::spawnerObject->buildingCosts.Contains(building)) return GetMinScore();

	EBuildingSize buildingSize = AGlobalSpawner::spawnerObject->buildingCosts[building].size;
	
	return FactorInversion(hex->CanBuildOnHex(buildingSize));
}
