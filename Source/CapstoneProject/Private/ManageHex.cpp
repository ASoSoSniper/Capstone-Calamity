// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageHex.h"

#include "BasePlayerController.h"

void UManageHex::Select(AActor* selectedObject)
{
}

void UManageHex::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, save in hex pointer
	case ObjectTypes::Hex:
		controller->selectedHex = objectType.hex;
		break;
		//If troop, save in troop pointer and switch to TroopManage state
	case ObjectTypes::MoveAI:
		controller->currentActionState = ActionStates::TroopManage;
		controller->actionStates[ActionStates::TroopManage]->Select(objectType.actor);
		break;
		//If building, switch to BaseManage state
	case ObjectTypes::Building:
		controller->currentActionState = ActionStates::BaseManage;
		break;
	}
}

void UManageHex::Reset()
{
}



void UManageHex::Action1()
{
	controller->Build(controller->buildingPrefab);
}

void UManageHex::Action2()
{
}

void UManageHex::Action3()
{
}

void UManageHex::Action4()
{
}
