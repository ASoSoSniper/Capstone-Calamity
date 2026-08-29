// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "UnitActions.h"
#include "UAIC_BuildingRatio.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_BuildingRatio : public UAI_Condition
{
	GENERATED_BODY()

public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Buildings") TSet<SpawnableBuildings> targetBuildings;
	UPROPERTY(EditAnywhere, Category = "Buildings") TSet<SpawnableBuildings> referenceBuildings;
	UPROPERTY(EditAnywhere, Category = "Buildings", meta = (ClampMin = 0.f)) float targetRatio = 1.f;
};
