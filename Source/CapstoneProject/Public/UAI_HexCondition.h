// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "BaseHex.h"
#include "UAI_HexCondition.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class CAPSTONEPROJECT_API UAI_HexCondition : public UAI_Condition
{
	GENERATED_BODY()

public:
	virtual float ScoreCondition(ABaseHex* hex) const PURE_VIRTUAL(UAI_HexCondition::ScoreCondition, return 0.f;);
};
