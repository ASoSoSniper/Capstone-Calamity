// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageMode.h"
#include "BasePlayerController.h"

void UManageMode::Select(AActor* selectedObject)
{
}

void UManageMode::SwitchState(UnitActions::SelectionIdentity& info)
{
	//Current selected object type:
	switch (info.type)
	{
		//If hex, save in hex pointer
	case ObjectTypes::Hex:
		controller->selectedHex = info.hex;
		break;
		//If troop, save in troop pointer and switch to TroopManage state
	case ObjectTypes::MoveAI:
		controller->currentActionState = ActionStates::TroopManage;
		controller->actionStates[ActionStates::TroopManage]->Select(info.actor);
		break;
		//If building, switch to BaseManage state
	case ObjectTypes::Building:
		controller->currentActionState = ActionStates::BaseManage;
		break;
	}

}

void UManageMode::DestroyObject(AActor* selectedObject)
{
	selectedObject->Destroy();
}

void UManageMode::Upgrade(AActor* selectedObject)
{
}
