// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_ResourceCheck.h"
#include "FactionController.h"
#include "Faction.h"

float UUAIC_ResourceCheck::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
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
