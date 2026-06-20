// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_TroopCondition.h"
#include "UTC_DistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUTC_DistanceToTarget : public UAI_TroopCondition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(ATroop* troop, ABaseHex* hex) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Troop") int maxDistance = 100;
};
