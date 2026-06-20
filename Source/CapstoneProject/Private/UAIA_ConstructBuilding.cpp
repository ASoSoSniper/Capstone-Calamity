// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_ConstructBuilding.h"
#include "Faction.h"
#include "GlobalSpawner.h"

void UUAIA_ConstructBuilding::ExecuteAction(IUAI_Controller* controller)
{
	UFaction* faction = controller->GetFaction();
	if (!faction) return;

	ABaseHex* bestHex = faction->GetPriorityHex_Building(building);
	if (!bestHex) return;

	AGlobalSpawner::spawnerObject->SpawnBuilding(faction->GetFaction(), building, bestHex);
}
