// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_BuildingRatio.h"
#include "Faction.h"

float UUAIC_BuildingRatio::ScoreCondition(IUAI_Controller* controller) const
{
    UFaction* faction = controller->GetFaction();
    
    auto buildingCount = [&](const TSet<SpawnableBuildings>& buildingTypes) -> int {
        int count = 0;
        for (SpawnableBuildings b : buildingTypes)
            count += faction->GetBuildingsOfType(b).Num();
        return count;
    };

    int refCount = buildingCount(referenceBuildings);

    if (refCount == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Reference building type invalid"));
        return GetMinScore();
    }
     
    int targetCount = buildingCount(targetBuildings);

    float alpha = static_cast<float>(targetCount) / refCount;

    return FactorInversion(EvaluateOnCurve(1 - FMath::Min(alpha, targetRatio) / targetRatio));
}
