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

	ABaseHex* bestHex = faction->GetPriorityHex_Workers(resourceTarget);
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
	
	bestHex->SetWorkers(WorkerType::Alien, amount);
}