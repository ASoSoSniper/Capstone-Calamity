// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UAI_Controller.h"
#include "UAI_Condition.generated.h"

UCLASS(Abstract, BlueprintType)
class CAPSTONEPROJECT_API UAI_Condition : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual float ScoreCondition(IUAI_Controller* controller) const PURE_VIRTUAL(UAI_Condition::ScoreCondition, return 0.f;);

protected:
	float FactorInversion(const float& score) const;
	float EvaluateOnCurve(const float& score) const;
	float GetMinScore() const;

private:
	UPROPERTY(EditAnywhere, Category = "Scoring") float minimumScore = 0.f;
	UPROPERTY(EditAnywhere, Category = "Scoring") bool bInvertScore = false;
	UPROPERTY(EditAnywhere, Category = "Scoring") UCurveFloat* scoreCurve = nullptr;
};
