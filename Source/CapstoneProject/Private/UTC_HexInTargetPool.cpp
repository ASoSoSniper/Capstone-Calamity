// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_HexInTargetPool.h"

float UUTC_HexInTargetPool::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	const TMap<ABaseHex*, EFactions>& targetPool = faction->GetTargetPool();

	if (targetPool.Contains(hex)) return FactorInversion(1.f);

	return GetMinScore();
}
