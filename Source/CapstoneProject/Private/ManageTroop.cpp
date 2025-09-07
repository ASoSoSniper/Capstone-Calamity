// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageTroop.h"
#include "BasePlayerController.h"
#include "BattleObject.h"

void UManageTroop::Select(AActor* selectedObject)
{
	if (selectedTroop)
	{
		HighlightSelected(selectedTroop, false);
		selectedTroop->selectedByPlayer = false;
	}

	if (!selectedObject) return;

	selectedTroop = Cast<ATroop>(selectedObject);
	controller->PlayUITroopSelectionSound(selectedTroop->GetUnitData()->GetFaction());

	if (selectedObject)
	{
		HighlightSelected(selectedObject, true);
		selectedTroop->selectedByPlayer = true;
	}
}

void UManageTroop::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	UHexNav* hexNav = objectType.actor->GetComponentByClass<UHexNav>();

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, set troop's destination to the hex
	case ObjectTypes::Hex:
		if (objectType.hex != selectedTroop->hexNav->GetCurrentHex()) CueActionState(ActionStates::HexManage, objectType.actor);
		else
		{
			AdvanceSelectCycle(objectType.hex);
		}
		break;

		//If troop (and hostile), set troop's destination to that troop's current hex
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage, objectType.actor);
		break;

		//If building (and hostile), set troop's destination to that building's current hex
	case ObjectTypes::Building:
		if (UnitActions::IsHostileTarget(selectedTroop->GetUnitData()->GetFaction(), objectType.building->GetUnitData()->GetFaction()))
		{
			if (hexNav)
				selectedTroop->SetDestination(hexNav->GetCurrentHex());
		}
		else
		{
			//else switch to BaseManage state
			controller->selectedWorldObject = objectType.building->hexNav->GetCurrentHex();
			CueActionState(ActionStates::HexManage, objectType.building->hexNav->GetCurrentHex());
			return;
		}
		break;

	case ObjectTypes::Battle:
		//EngagementSelect engage = UnitActions::DetermineConflictAlignment(selectedTroop->GetUnitData()->faction, objectType.battle->currentBattle.Group1, objectType.battle->currentBattle.Group2);
		CueActionState(ActionStates::BattleManage, objectType.actor);
		break;
	}

	subSelect = None;
}

void UManageTroop::Reset()
{
	subSelect = None;
	if (selectedTroop)
	{
		HighlightSelected(selectedTroop, false);
		selectedTroop->selectedByPlayer = false;
	}
	selectedTroop = nullptr;
}

void UManageTroop::CheckSelection()
{
	if (selectedTroop == nullptr)
	{
		CueActionState(ActionStates::None);
		return;
	}

	if (selectedTroop->interact->canInteract == false)
	{
		CueActionState(ActionStates::None);
	}
}

AActor* UManageTroop::GetSelectedObject()
{
	return selectedTroop;
}

void UManageTroop::CommandToMerge(ATroop* troop, AActor* target)
{
	troop->CommandTroopToMerge(target);

	CueActionState(ActionStates::None);
}

void UManageTroop::Action1()
{
	selectedTroop->Action1();
}

//Army Merging Toggle
void UManageTroop::Action2()
{
	switch (subSelect)
	{
	case Merge:
		subSelect = None;
		break;
	default:
		subSelect = Merge;
		break;
	}
}

void UManageTroop::Action3()
{
	selectedTroop->Action3();
}

void UManageTroop::Action4()
{
	
}

void UManageTroop::CommandAction()
{
	if (!controller->hoveredWorldObject || !selectedTroop || selectedTroop->GetUnitData()->GetFaction() != Factions::Human) return;

	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->hoveredWorldObject);
	UHexNav* hexNav = objectType.actor->GetComponentByClass<UHexNav>();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Command Action Triggered"));
	switch (objectType.type)
	{
	case ObjectTypes::Hex:
		//if (!UnitActions::HexHasFriendlyTroop(controller->playerFaction, objectType.actor))
		selectedTroop->SetDestination(objectType.actor);
		break;
	case ObjectTypes::Building:
		if (hexNav)
		{
			if (!UnitActions::HexHasFriendlyTroop(controller->playerFaction, hexNav->GetCurrentHex()))
				selectedTroop->SetDestination(hexNav->GetCurrentHex());
		}
		break;
	case ObjectTypes::Battle:
		if (hexNav)
			selectedTroop->SetDestination(hexNav->GetCurrentHex());
	case ObjectTypes::MoveAI:
		if (UnitActions::IsHostileTarget(selectedTroop->GetUnitData()->GetFaction(), objectType.building->GetUnitData()->GetFaction()))
		{
			if (hexNav)
				selectedTroop->SetDestination(hexNav->GetCurrentHex());
		}
		else
		{
			//Check subselection mode
			switch (subSelect)
			{
			case Merge:
				//Move toward friendly troop with the intent to merge
				if (hexNav && selectedTroop != objectType.actor)
				{
					CommandToMerge(selectedTroop, objectType.moveAI);
					//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Merging"));
					return;
				}
				else
				{
					return;
				}
				break;
			}

		}
	}
}
