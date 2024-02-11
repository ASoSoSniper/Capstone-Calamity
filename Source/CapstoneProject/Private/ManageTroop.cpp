// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageTroop.h"
#include "BasePlayerController.h"
#include "BattleObject.h"

void UManageTroop::Select(AActor* selectedObject)
{
	if (selectedTroop) HighlightSelected(selectedTroop, false);

	if (!selectedObject) return;

	selectedTroop = Cast<ATroop>(selectedObject);

	if (selectedObject) HighlightSelected(selectedObject, true);
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
		switch (subSelect)
		{
		case None:
			selectedTroop->hexNav->targetHex = objectType.actor;
			break;
		case Merge:
			if (controller->OutpostCanStoreTroops())
			{
				selectedTroop->hexNav->targetHex = objectType.actor;

				if (selectedTroop) CommandToMerge(selectedTroop, controller->GetOutpost());
			}
		}
		break;

		//If troop (and hostile), set troop's destination to that troop's current hex
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
				Select(objectType.actor);
				return;
			case Merge:
				//Move toward friendly troop with the intent to merge
				if (hexNav && selectedTroop != objectType.actor)
				{
					selectedTroop->hexNav->targetHex = hexNav->currentHex;

					if (selectedTroop) CommandToMerge(selectedTroop, objectType.moveAI);
				}
				else
				{
					return;
				}
				break;
			}
			
		}
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
			CueActionState(ActionStates::BaseManage, objectType.actor);

			return;
		}
		break;

	case ObjectTypes::Battle:
		EngagementSelect engage = UnitActions::DetermineConflictAlignment(selectedTroop->unitStats->faction, objectType.battle->currentBattle.Group1, objectType.battle->currentBattle.Group2);
		switch (engage)
		{
		case EngagementSelect::DoNotJoin:
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Cannot join this battle"));
			return;
		default:
			if (hexNav)
				selectedTroop->hexNav->targetHex = hexNav->currentHex;
			break;
		}
		break;
	}

	subSelect = None;
	if (selectedTroop->hexNav->targetHex)
		selectedTroop->CreatePath();
}

void UManageTroop::Reset()
{
	subSelect = None;
	HighlightSelected(selectedTroop, false);
}

void UManageTroop::CheckSelection()
{
	if (selectedTroop == nullptr)
	{
		CueActionState(ActionStates::None);
	}

	if (selectedTroop->interact->canInteract == false)
	{
		CueActionState(ActionStates::None);
	}
}

void UManageTroop::CommandToMerge(ATroop* troop, AActor* target)
{
	troop->merging = true;
	troop->targetToMerge = target;
	CueActionState(ActionStates::None);
}

void UManageTroop::Action1()
{
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
