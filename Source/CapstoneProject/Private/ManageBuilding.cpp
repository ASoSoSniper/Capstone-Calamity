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

void UManageBuilding::Action1()
{
	switch (subSelect)
	{
	case None:
		subSelect = Build;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("WHYYYYYYYYYYYYYYYYYYYYYY"));
		break;
	case Build:
		selectedBuilding->Action1();
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("WHYYYYYYYYYYYYYYYYYYYYYY"));
		break;
	}
}

void UManageBuilding::Action2()
{
	switch (subSelect)
	{
	case Build:
		selectedBuilding->Action2();
		break;
	}
}

void UManageBuilding::Action3()
{
	switch (subSelect)
	{
	case Build:
		selectedBuilding->Action3();
		break;
	}
}

void UManageBuilding::Action4()
{
	switch (subSelect)
	{
	case Build:
		selectedBuilding->Action4();
		break;
	}
}

void UManageBuilding::Action5()
{
	switch (subSelect)
	{
	case Build:
		selectedBuilding->Action5();
		break;
	}
}


void UManageBuilding::Reset()
{
	subSelect = None;
}
