// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_TroopBuildingRatio.h"
#include "Faction.h"
#include "TroopFactory.h"

float UUAIC_TroopBuildingRatio::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	int troopCount = faction->GetTroops().Num();
	int buildingCount = faction->GetBuildingCount();

	if (buildingCount == 0) return GetMinScore();
	if (factorTroopsInConstruction)
	{
		for (ABuilding* building : faction->GetBuildingsOfType(SpawnableBuildings::RobotFactory))
		{
			ATroopFactory* factory = Cast<ATroopFactory>(building);
			if (factory) troopCount += factory->GetQueueSize();
		}
	}

	float alpha = static_cast<float>(troopCount) / buildingCount;

	return EvaluateOnCurve(FactorInversion(1 - FMath::Min(alpha, targetRatio) / targetRatio));
}
