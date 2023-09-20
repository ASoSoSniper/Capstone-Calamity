// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageTroop.h"
#include "BasePlayerController.h"

void UManageTroop::Select(AActor* selectedObject)
{
	selectedTroop = Cast<AMovementAI>(selectedObject);
}

void UManageTroop::SwitchState(UnitActions::SelectionIdentity& info)
{
	bool hostileTarget = UnitActions::IsHostileTarget(selectedTroop, info.actor);

	//Current selected object type:
	switch (info.type)
	{
		//If hex, set troop's destination to the hex
	case ObjectTypes::Hex:
		selectedTroop->hexNav->targetHex = info.actor;
		break;

		//If troop (and hostile), set troop's destination to that troop's current hex
	case ObjectTypes::MoveAI:
		if (hostileTarget)
		{
			UHexNav* hexNav = controller->selectedWorldObject->GetComponentByClass<UHexNav>();
			if (hexNav)
				selectedTroop->hexNav->targetHex = hexNav->currentHex;
		}
		else
		{
			//else set selected troop to that (friendly) troop
			selectedTroop = info.moveAI;
			return;
		}
		break;

		//If building (and hostile), set troop's destination to that building's current hex
	case ObjectTypes::Building:
		if (hostileTarget)
		{
			UHexNav* hexNav = info.actor->GetComponentByClass<UHexNav>();
			if (hexNav)
				selectedTroop->hexNav->targetHex = hexNav->currentHex;
		}
		else
		{
			//else switch to BaseManage state
			controller->currentActionState = ActionStates::BaseManage;
			controller->actionStates[ActionStates::BaseManage]->Select(info.actor);
			return;
		}
		break;
	}

	selectedTroop->CreatePath();
}
