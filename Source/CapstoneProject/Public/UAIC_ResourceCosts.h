// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "StratResources.h"
#include "UAIC_ResourceCosts.generated.h"

/**
 * Calculates the gain/loss ratio of the resource income and scores higher the further it is from the target ratio.
 * 0 = gain/cost ratio matches or exceeds the target (i.e. target is 2 and ratio is 2 : 1).
 * 1 = ratio falls below the target (i.e. target is 2 and ratio is 0 : 1).
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_ResourceCosts : public UAI_Condition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Resources") EStratResources resource;
	UPROPERTY(EditAnywhere, Category = "Resources") bool includeIncompleteBuildings = false;
	UPROPERTY(EditAnywhere, Category = "Resources") float targetRatio = 2.f;
};
