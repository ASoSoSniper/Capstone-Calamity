// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "UAIC_CanBuildOfSize.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_CanBuildOfSize : public UAI_Condition
{
	GENERATED_BODY()

public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Building") int buildingSize = 0;
};
