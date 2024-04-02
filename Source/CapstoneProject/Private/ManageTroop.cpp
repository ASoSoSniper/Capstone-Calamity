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

	if (selectedObject)
	{
		HighlightSelected(selectedObject, true);
		selectedTroop->selectedByPlayer = true;
	}
}

void UManageTroop::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	bool hostileTarget = UnitActions::IsHostileTarget(selectedTroop, objectType.actor);
	UHexNav* hexNav = objectType.actor->GetComponentByClass<UHexNav>();

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, set troop's destination to the hex
	case ObjectTypes::Hex:
		CueActionState(ActionStates::HexManage, objectType.actor);
		break;

		//If troop (and hostile), set troop's destination to that troop's current hex
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage, objectType.actor);
		break;

		//If building (and hostile), set troop's destination to that building's current hex
	case ObjectTypes::Building:
		if (hostileTarget)
		{
			if (hexNav)
				selectedTroop->hexNav->targetHex = hexNav->currentHex;
		}
		else
		{
			//else switch to BaseManage state
			controller->selectedWorldObject = objectType.building->hexNav->currentHex;
			CueActionState(ActionStates::HexManage, objectType.building->hexNav->currentHex);
			return;
		}
		break;

	case ObjectTypes::Battle:
		EngagementSelect engage = UnitActions::DetermineConflictAlignment(selectedTroop->unitStats->faction, objectType.battle->currentBattle.Group1, objectType.battle->currentBattle.Group2);
		switch (engage)
		{
		case EngagementSelect::DoNotJoin:
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Cannot join this battle"));
			return;
		default:
			if (hexNav)
				selectedTroop->hexNav->targetHex = hexNav->currentHex;
			break;
		}
		break;
	}

	subSelect = None;
	if (!selectedTroop) return;

	if (selectedTroop->hexNav->targetHex)
	{
		selectedTroop->CreatePath();
		//controller->Deselect();
	}
		
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
	UHexNav* hexNav = target->GetComponentByClass<UHexNav>();
	UUnitStats* targetStats = target->GetComponentByClass<UUnitStats>();

	if (troop->unitStats->savedUnits.Num() + targetStats->savedUnits.Num() > troop->armyCap) return;

	troop->hexNav->targetHex = hexNav->currentHex;

	troop->merging = true;
	troop->targetToMerge = target;

	if (troop->hexNav->targetHex)
		troop->CreatePath();
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
	if (!controller->hoveredWorldObject || !selectedTroop) return;

	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->hoveredWorldObject);
	bool hostileTarget = UnitActions::IsHostileTarget(selectedTroop, objectType.actor);
	UHexNav* hexNav = objectType.actor->GetComponentByClass<UHexNav>();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Command Action Triggered"));
	switch (objectType.type)
	{
	case ObjectTypes::Hex:
		if (!UnitActions::HexHasFriendlyTroop(controller->playerFaction, objectType.actor))
			selectedTroop->hexNav->targetHex = objectType.actor;
		break;
	case ObjectTypes::MoveAI:
		if (hostileTarget)
		{
			if (hexNav)
				selectedTroop->hexNav->targetHex = hexNav->currentHex;
		}
		else
		{
			//Check subselection mode
			switch (subSelect)
			{
			case None:
				//Set selected troop to that (friendly) troop
				//Select(objectType.actor);
				return;
			case Merge:
				//Move toward friendly troop with the intent to merge
				if (hexNav && selectedTroop != objectType.actor)
				{
					CommandToMerge(selectedTroop, objectType.moveAI);
					GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Merging"));
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

	if (selectedTroop->hexNav->targetHex)
	{
		selectedTroop->CreatePath();
	}
}
