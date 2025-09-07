// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageHex.h"
#include "BasePlayerController.h"
#include "GlobalSpawner.h"

void UManageHex::Select(AActor* selectedObject)
{
	//Un-highlight the previously selected hex
	if (controller->selectedHex) HighlightSelected(controller->selectedHex, false);
	
	//Set new hex to selected hex
	ABaseHex* hex = Cast<ABaseHex>(selectedObject);
	if (hex)
	{
		controller->selectedHex = hex;
		controller->PlayUIHexSound(hex->GetHexTerrain(), hex);
	}

	//Highlight new hex
	if (selectedObject) HighlightSelected(selectedObject, true);

	//(Debug) Display the coordinates of the selected hex
	/*FVector2D coords = AGlobalSpawner::spawnerObject->GetHexCoordinates(hex);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("(ManageHex) Selected hex [%d, %d]"), FMath::RoundToInt(coords.X), FMath::RoundToInt(coords.Y)));*/
}

void UManageHex::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, save in hex pointer
	case ObjectTypes::Hex:
		if (objectType.hex != controller->selectedHex) Select(objectType.actor);
		else
		{
			AdvanceSelectCycle(objectType.hex);
		}
		break;
		//If troop, save in troop pointer and switch to TroopManage state
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage, objectType.actor);
		break;
		//If building, switch to BaseManage state
	case ObjectTypes::Building:
		controller->selectedWorldObject = objectType.building->hexNav->GetCurrentHex();
		CueActionState(ActionStates::HexManage, objectType.building->hexNav->GetCurrentHex());
		break;
	case ObjectTypes::Battle:
		CueActionState(ActionStates::BattleManage, objectType.actor);
		break;
	}
}

void UManageHex::Reset()
{
	subSelect = None;
	if (controller->selectedHex)
	{
		HighlightSelected(controller->selectedHex, false);
	}
}



void UManageHex::Action1()
{
	
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

void UManageHex::Action5()
{
	
}

void UManageHex::Action6()
{
	
}

void UManageHex::Action7()
{
	
}

void UManageHex::Action8()
{
	
}

void UManageHex::Action9()
{
	
}

void UManageHex::Action10()
{
	
}

void UManageHex::Action11()
{
	
}

void UManageHex::Action12()
{
}

void UManageHex::AddWorkersToHex(WorkerType worker, int workers)
{
	controller->selectedHex->workersInHex[worker] += UnitActions::AddWorkers(controller->playerFaction, worker, workers, controller->selectedHex);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("%d workers in hex"), controller->selectedHex->workersInHex[worker]));
}

void UManageHex::RemoveWorkers(WorkerType worker, int workers)
{
	controller->selectedHex->workersInHex[worker] -= UnitActions::RemoveWorkers(controller->playerFaction, worker, workers, controller->selectedHex);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("%d workers in hex"), controller->selectedHex->workersInHex[worker]));
}
