// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_ResourceCompare.h"
#include "FactionController.h"
#include "Faction.h"

float UUAIC_ResourceCompare::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return GetMinScore();

	const TMap<EStratResources, int>& inventory = faction->GetNetResourcesPerDay(includeIncompleteBuildings);

	if (greatestCount)
	{
		EStratResources greatest = EStratResources::None;
		EStratResources secondGreatest = EStratResources::None;

		for (const TPair<EStratResources, int>& r : inventory)
		{
			if (r.Key == EStratResources::Wealth && !includeWealth) continue;

			if (greatest == EStratResources::None || 
				r.Value > inventory[greatest])
			{
				secondGreatest = greatest;
				greatest = r.Key;
			}
			else if (secondGreatest == EStratResources::None || 
				(r.Value > inventory[secondGreatest] && r.Key != greatest))
			{
				secondGreatest = r.Key;
			}
		}

		if (greatest != resource) return FactorInversion(0.f);

		float alpha = (float)inventory[secondGreatest] / (float)inventory[greatest];

		return EvaluateOnCurve(FactorInversion(1.f - alpha));
	}
	else
	{
		EStratResources smallest = EStratResources::None;
		EStratResources secondSmallest = EStratResources::None;

		for (const TPair<EStratResources, int>& r : inventory)
		{
			if (r.Key == EStratResources::Wealth && !includeWealth) continue;

			if (smallest == EStratResources::None || 
				r.Value < inventory[smallest])
			{
				secondSmallest = smallest;
				smallest = r.Key;
			}
			else if (secondSmallest == EStratResources::None || 
				(r.Value < inventory[secondSmallest] && r.Key != smallest))
			{
				secondSmallest = r.Key;
			}
		}

		if (smallest != resource) return GetMinScore();

		float alpha = (float)inventory[smallest] / (float)inventory[secondSmallest];

		return EvaluateOnCurve(FactorInversion(1.f - alpha));
	}
}
