// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_ResourceCheck.h"
#include "Faction.h"

float UUAIC_ResourceCheck::ScoreCondition(IUAI_Controller* controller) const
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return GetMinScore();

	const TMap<EStratResources, FInventoryStat>& inventory = faction->resourceInventory;

	for (const TPair<EStratResources, FResourceSetting>& resource : resourcesToCheck)
	{
		switch (resource.Key)
		{
		case EStratResources::None:
			continue;
		case EStratResources::Population:
			continue;
		default:
			if (resource.Value.greaterEqual)
			{
				if (inventory[resource.Key].currentResources < resource.Value.value) return GetMinScore();
			}
			else
			{
				if (inventory[resource.Key].currentResources > resource.Value.value) return GetMinScore();
			}
		}
	}

	return FactorInversion(1.f);
}
