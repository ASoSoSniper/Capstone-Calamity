// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageTroop.h"
#include "BasePlayerController.h"
#include "BattleObject.h"

void UManageTroop::Select(AActor* selectedObject)
{
	selectedTroop = Cast<ATroop>(selectedObject);
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
		selectedTroop->hexNav->targetHex = objectType.actor;
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
				selectedTroop = objectType.moveAI;
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
			CueActionState(ActionStates::BaseManage);
			controller->actionStates[ActionStates::BaseManage]->Select(objectType.actor);

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
	selectedTroop->CreatePath();
}

void UManageTroop::Reset()
{
	Super::Reset();
	subSelect = None;
}

void UManageTroop::CheckSelection()
{
	if (selectedTroop == nullptr)
	{
		CueActionState(ActionStates::None);
	}
}

void UManageTroop::CommandToMerge(ATroop* troop, ATroop* targetTroop)
{
	troop->merging = true;
	troop->targetToMerge = targetTroop;
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
