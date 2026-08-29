// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_BuildingRatio.h"
#include "Faction.h"

float UUAIC_BuildingRatio::ScoreCondition(IUAI_Controller* controller) const
{
    UFaction* faction = controller->GetFaction();
    
    auto countOutpostLikes = [&](SpawnableBuildings buildingType, int& outCount) {
            if (buildingType == SpawnableBuildings::Outpost) {
                outCount += faction->GetBuildingsOfType(SpawnableBuildings::RockCity).Num();
                outCount += faction->GetBuildingsOfType(SpawnableBuildings::Capitol).Num();
                outCount += faction->GetBuildingsOfType(SpawnableBuildings::AlienCity).Num();
            }
        };

    int refCount = faction->GetBuildingsOfType(referenceBuilding).Num();
    countOutpostLikes(referenceBuilding, refCount);

    if (refCount == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Reference building type invalid"));
        return GetMinScore();
    }
     
    int compareCount = faction->GetBuildingsOfType(comparedBuilding).Num();
    countOutpostLikes(comparedBuilding, compareCount);

    float alpha = static_cast<float>(compareCount) / refCount;

    return FactorInversion(EvaluateOnCurve(1 - FMath::Min(alpha, targetRatio) / targetRatio));
}
