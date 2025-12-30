// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_ConstructBuilding.h"
#include "FactionController.h"
#include "Faction.h"
#include "GlobalSpawner.h"

void UUAIA_ConstructBuilding::ExecuteAction(IUAI_Controller* controller)
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return;

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return;
	if (faction->ownedHexes.IsEmpty()) return;

	EStratResources buildingResource = AGlobalSpawner::GetMainBuildingYield(building);
	if (buildingResource == EStratResources::None) return;

	int buildingSize = AGlobalSpawner::spawnerObject->buildingCosts[building].hexLayers;

	ABaseHex* bestHex = nullptr;
	int bestResource = 0;
	for (ABaseHex*& hex : faction->ownedHexes)
	{
		if (hex->CanBuildOnHex(buildingSize))
		{
			int resource = hex->GetHexResources()[buildingResource];

			if (!bestHex || resource > bestResource)
			{
				bestHex = hex;
				bestResource = resource;
			}
		}
	}

	if (!bestHex) return;

	AGlobalSpawner::spawnerObject->SpawnBuilding(faction->GetFaction(), building, bestHex);
}
