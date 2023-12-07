// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageMode.h"
#include "BasePlayerController.h"

void UManageMode::Select(AActor* selectedObject)
{
}

void UManageMode::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, save in hex pointer
	case ObjectTypes::Hex:
		controller->selectedHex = objectType.hex;
		CueActionState(ActionStates::HexManage);
		break;
		//If troop, save in troop pointer and switch to TroopManage state
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage);
		controller->actionStates[ActionStates::TroopManage]->Select(objectType.actor);
		break;
		//If building, switch to BaseManage state
	case ObjectTypes::Building:
		CueActionState(ActionStates::BaseManage);
		controller->actionStates[ActionStates::BaseManage]->Select(objectType.actor);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Base Manage Mode"));
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

void UManageMode::Reset()
{
}

void UManageMode::CheckSelection()
{
}

void UManageMode::CueActionState(ActionStates nextState)
{
	controller->currentActionState = nextState;
}

void UManageMode::Action1()
{
}

void UManageMode::Action2()
{
}

void UManageMode::Action3()
{
}

void UManageMode::Action4()
{
}

void UManageMode::Action5()
{
}

void UManageMode::Action6()
{
}

void UManageMode::Action7()
{
}

void UManageMode::Action8()
{
}

void UManageMode::Action9()
{
}

void UManageMode::Action10()
{
}

void UManageMode::Action11()
{
}

void UManageMode::Action12()
{
}
