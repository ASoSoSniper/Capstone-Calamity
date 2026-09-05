// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "UAIC_TroopBuildingRatio.generated.h"

/**
 * Calculates the troop-to-building ratio of the resource income and scores higher the further it is from the target ratio.
 * 0 = ratio matches or exceeds the target (i.e. target is 2 and ratio is 2 : 1).
 * 1 = ratio falls below the target (i.e. target is 2 and ratio is 0 : 1).
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_TroopBuildingRatio : public UAI_Condition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Ratio") float targetRatio = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Ratio") bool factorTroopsInConstruction = true;
};
