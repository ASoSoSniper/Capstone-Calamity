// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_SetWorkersOnHex.h"
#include "FactionController.h"
#include "Faction.h"
#include "UnitActions.h"

void UUAIA_SetWorkersOnHex::ExecuteAction(IUAI_Controller* controller)
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return;

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return;
	if (faction->ownedHexes.IsEmpty()) return;

	ABaseHex* bestHex = nullptr;
	int bestResource = 0;
	for (ABaseHex*& hex : faction->ownedHexes)
	{
		if (hex->workersInHex[WorkerType::Alien] >= hex->GetMaxWorkers()) continue;
		int resource = hex->GetHexResources()[resourceTarget];

		if (!bestHex || resource > bestResource)
		{
			bestHex = hex;
			bestResource = resource;
		}
	}

	if (!bestHex) return;

	int amount = 0;
	switch (workerCount)
	{
	case EWorkersToSet::Fill:
		amount = bestHex->GetMaxWorkers();
		break;
	case EWorkersToSet::Empty:
		amount = -bestHex->GetMaxWorkers();
		break;
	case EWorkersToSet::SetAmount:
		amount = setAmount;
		break;
	}
	
	UnitActions::SetWorkers(faction->GetFaction(), WorkerType::Alien, amount, bestHex);
}