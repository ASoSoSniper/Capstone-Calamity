// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_HexDistanceToOutpost.h"
#include "stdlib.h"

float UUTC_HexDistanceToOutpost::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	TSet<ABuilding*> allOutpostLikes;
	allOutpostLikes.Append(faction->GetBuildingsOfType(SpawnableBuildings::Outpost));
	allOutpostLikes.Append(faction->GetBuildingsOfType(SpawnableBuildings::Capitol));
	allOutpostLikes.Append(faction->GetBuildingsOfType(SpawnableBuildings::RockCity));
	allOutpostLikes.Append(faction->GetBuildingsOfType(SpawnableBuildings::AlienCity));

	float closest = std::numeric_limits<float>::infinity();
	ABaseHex* closestHex = nullptr;

	FVector hexPos = hex->GetActorLocation();

	for (ABuilding* building : allOutpostLikes)
	{
		ABaseHex* outpostHex = building->hexNav->GetCurrentHex();
		float dist = FVector::DistSquaredXY(outpostHex->GetActorLocation(), hexPos);

		if (dist < closest)
		{
			closest = dist;
			closestHex = outpostHex;
		}
	}

	if (!closestHex) return GetMinScore();

	float distSqrt = FMath::Sqrt(closest);
	float alpha = FMath::Clamp(1 - (distSqrt / maxDistance), 0.f, 1.f);

	return FactorInversion(EvaluateOnCurve(alpha));
}
