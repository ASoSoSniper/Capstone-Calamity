// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_TroopCondition.h"
#include "UTC_TroopsTargetingHex.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUTC_TroopsTargetingHex : public UAI_TroopCondition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(ATroop* troop, ABaseHex* hex) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Troops") int maxTroops = 1;
};
