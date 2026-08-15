// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_RouteProgress.h"

float UUTC_RouteProgress::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (!troop) return GetMinScore();
	if (!troop->IsTraveling()) return FactorInversion(1.f);

	const TArray<const ABaseHex*> route;
	int num = route.IsEmpty() ? 1 : route.Num();
	int index = troop->GetHexPathIndex();

	float alpha = static_cast<float>(index) / num;

	return EvaluateOnCurve(FactorInversion(alpha));
}
