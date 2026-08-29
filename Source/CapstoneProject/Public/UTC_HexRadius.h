// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_TroopCondition.h"
#include "UTC_HexRadius.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUTC_HexRadius : public UAI_TroopCondition
{
	GENERATED_BODY()

public:
	virtual float ScoreCondition(UFaction* faction, ABaseHex* hex) const override;
private:
	UPROPERTY(EditAnywhere, Category = "Radius", meta = (ClampMin = 0)) int radius = 0;
};
