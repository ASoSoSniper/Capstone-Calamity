// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageBuilding.h"
#include "BasePlayerController.h"

void UManageBuilding::Select(AActor* selectedObject)
{
	selectedBuilding = Cast<ABuilding>(selectedObject);
}

void UManageBuilding::SwitchState(UnitActions::SelectionIdentity& info)
{
	//Current selected object type:
	switch (info.type)
	{
		//If hex, switch to None state
	case ObjectTypes::Hex:		
		controller->currentActionState = ActionStates::None;
		controller->selectedHex = info.hex;
		break;
		//If troop, set to TroopManage state
	case ObjectTypes::MoveAI:
		controller->currentActionState = ActionStates::TroopManage;
		controller->actionStates[ActionStates::TroopManage]->Select(info.actor);
		break;

	case ObjectTypes::Building:
		selectedBuilding = info.building;
		break;
	}
}
