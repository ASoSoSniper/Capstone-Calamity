// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_Condition.h"

float UAI_Condition::FactorInversion(const float& score) const
{
    float clampedScore = FMath::Clamp(score, 0.f, 1.f);

    if (!bInvertScore) return clampedScore;

    return FMath::Clamp(1.f - clampedScore, 0.f, 1.f);
}

float UAI_Condition::EvaluateOnCurve(const float& score) const
{
    float clampedScore = FMath::Clamp(score, 0.f, 1.f);

    if (!scoreCurve) return clampedScore;

    return scoreCurve->GetFloatValue(clampedScore);
}
