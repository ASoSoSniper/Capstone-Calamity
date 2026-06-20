// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_DistanceToTarget.h"
#include "BaseHex.h"

float UUTC_DistanceToTarget::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (!troop) return FactorInversion(0.f);

	FVector2D troopPos = troop->hexNav->GetCurrentHex()->GetHexCoordinates();
	FVector2D hexPos = hex->GetHexCoordinates();

	TArray<const ABaseHex*> path = troop->SimulatePathToHex(hex);
	float alpha = static_cast<float>(path.Num()) / maxDistance;

	return FactorInversion(EvaluateOnCurve(alpha));
}
