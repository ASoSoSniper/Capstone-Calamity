// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageBuilding.h"
#include "BasePlayerController.h"

void UManageBuilding::Select(AActor* selectedObject)
{
	selectedBuilding = Cast<ABuilding>(selectedObject);
}

void UManageBuilding::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, switch to None state
	case ObjectTypes::Hex:		
		CueActionState(ActionStates::None);
		controller->selectedHex = objectType.hex;
		break;
		//If troop, set to TroopManage state
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage);
		controller->actionStates[ActionStates::TroopManage]->Select(objectType.actor);
		break;

	case ObjectTypes::Building:
		selectedBuilding = objectType.building;
		break;
	}
}

void UManageBuilding::CheckSelection()
{
	if (selectedBuilding == nullptr)
	{
		CueActionState(ActionStates::None);
	}
}
