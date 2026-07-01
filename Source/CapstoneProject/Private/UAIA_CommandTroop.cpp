// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_CommandTroop.h"
#include "Faction.h"
#include "FactionController.h"

void UUAIA_CommandTroop::ExecuteAction(IUAI_Controller* controller)
{
	if (!troopAction) return;

	AFactionController* factionController = controller->GetFaction()->GetFactionController();

	ATroop* priorityTroop = factionController->GetPriorityTroop_Troop();
	ABaseHex* priorityHex = factionController->GetPriorityTroop_TargetHex();

	if (!priorityTroop || !priorityHex) return;

	priorityTroop->AI_SetMovementAction(troopAction, priorityHex);
}
