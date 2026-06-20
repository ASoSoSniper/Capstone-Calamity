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
class CAPSTONEPROJECT_API UAI_HexCondition : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual float ScoreCondition(ABaseHex* hex) const PURE_VIRTUAL(UAI_HexCondition::ScoreCondition, return 0.f;);

protected:
	float FactorInversion(const float& score) const;
	float EvaluateOnCurve(const float& score) const;
	float GetMinScore() const;

private:
	UPROPERTY(EditAnywhere, Category = "Scoring") float minimumScore = 0.f;
	UPROPERTY(EditAnywhere, Category = "Scoring") bool bInvertScore = false;
	UPROPERTY(EditAnywhere, Category = "Scoring") UCurveFloat* scoreCurve = nullptr;
};
