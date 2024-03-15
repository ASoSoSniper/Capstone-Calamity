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
		CueActionState(ActionStates::HexManage, objectType.actor);
		break;
		//If troop, save in troop pointer and switch to TroopManage state
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage, objectType.actor);
		break;
		//If building, switch to BaseManage state
	case ObjectTypes::Building:
		controller->selectedWorldObject = objectType.building->hexNav->currentHex;
		CueActionState(ActionStates::HexManage, objectType.building->hexNav->currentHex);
		break;
	case ObjectTypes::Battle:
		CueActionState(ActionStates::BattleManage, objectType.actor);
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

void UManageMode::CueActionState(ActionStates nextState, AActor* selectedObject)
{
	Reset();
	controller->currentActionState = nextState;

	if (selectedObject && nextState != ActionStates::None)
	{
		controller->actionStates[nextState]->Select(selectedObject);
	}
}

void UManageMode::HighlightSelected(AActor* object, bool enable)
{
	if (!object) return;

	UMeshVisibility* visibility = object->GetComponentByClass<UMeshVisibility>();
	if (visibility) visibility->SetSelected(enable);
}

AActor* UManageMode::GetSelectedObject()
{
	return nullptr;
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
