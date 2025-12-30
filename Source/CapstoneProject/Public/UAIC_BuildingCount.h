// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "UnitActions.h"
#include "UAIC_BuildingCount.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_BuildingCount : public UAI_Condition
{
	GENERATED_BODY()
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Buildings") SpawnableBuildings buildingType;
	UPROPERTY(EditAnywhere, Category = "Buildings") int value = 0;
	UPROPERTY(EditAnywhere, Category = "Buildings") bool greaterEqual = true;
};
