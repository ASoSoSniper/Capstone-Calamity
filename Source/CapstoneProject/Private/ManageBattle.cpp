// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageBattle.h"
#include "BasePlayerController.h"

void UManageBattle::Select(AActor* selectedObject)
{
	if (selectedBattle)
	{
		HighlightSelected(selectedBattle, false);
		selectedBattle->selectedByPlayer = false;
	}

	selectedBattle = Cast<ABattleObject>(selectedObject);

	if (selectedBattle)
	{
		HighlightSelected(selectedObject, true);
		selectedBattle->selectedByPlayer = true;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Battle selected"));
}

void UManageBattle::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, switch to None state
	case ObjectTypes::Hex:
		if (objectType.hex != Cast<ABaseHex>(selectedBattle->hexNav->currentHex)) CueActionState(ActionStates::HexManage, objectType.actor);
		else
		{
			AdvanceSelectCycle(objectType.hex);
		}
		break;
		//If troop, set to TroopManage state
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage, objectType.actor);
		break;

	case ObjectTypes::Building:
		controller->selectedWorldObject = objectType.building->hexNav->currentHex;
		CueActionState(ActionStates::HexManage, objectType.building->hexNav->currentHex);
		break;

	case ObjectTypes::Battle:
		Select(objectType.actor);
		break;
	}
}

void UManageBattle::Reset()
{
	HighlightSelected(selectedBattle, false);
}

void UManageBattle::CheckSelection()
{
	if (selectedBattle == nullptr)
	{
		CueActionState(ActionStates::None);
	}
}

AActor* UManageBattle::GetSelectedObject()
{
	return selectedBattle;
}

void UManageBattle::Action1()
{
	selectedBattle->FleeFromBattle(Factions::Human);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("ACTION 1 TRIGGERED"));
}
