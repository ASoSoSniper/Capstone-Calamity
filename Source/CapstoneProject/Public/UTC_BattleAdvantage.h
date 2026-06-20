// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_TroopCondition.h"
#include "UTC_BattleAdvantage.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUTC_BattleAdvantage : public UAI_TroopCondition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(ATroop* troop, ABaseHex* hex) const override;
};
