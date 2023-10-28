// Fill out your copyright notice in the Description page of Project Settings.


#include "ManageHex.h"
#include "BasePlayerController.h"

void UManageHex::Select(AActor* selectedObject)
{
}

void UManageHex::SwitchState()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(controller->selectedWorldObject);

	//Current selected object type:
	switch (objectType.type)
	{
		//If hex, save in hex pointer
	case ObjectTypes::Hex:
		controller->selectedHex = objectType.hex;
		break;
		//If troop, save in troop pointer and switch to TroopManage state
	case ObjectTypes::MoveAI:
		CueActionState(ActionStates::TroopManage);
		controller->actionStates[ActionStates::TroopManage]->Select(objectType.actor);
		break;
		//If building, switch to BaseManage state
	case ObjectTypes::Building:
		CueActionState(ActionStates::BaseManage);
		break;
	}
}

void UManageHex::Reset()
{
}



void UManageHex::Action1()
{
	controller->Build(SpawnableBuildings::MiningStation);
}

void UManageHex::Action2()
{
	controller->Build(SpawnableBuildings::Farmland);
}

void UManageHex::Action3()
{
	AddWorkersToHex(WorkerType::Human, 2);
}

void UManageHex::Action4()
{
	RemoveWorkers(WorkerType::Human, 2);
}

void UManageHex::AddWorkersToHex(WorkerType worker, int workers)
{
	controller->selectedHex->workersInHex[worker] += UnitActions::AddWorkers(controller->playerFaction, worker, workers, controller->selectedHex->workersInHex[worker]);
	controller->selectedHex->ActiveHarvesting();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("%d workers in hex"), controller->selectedHex->workersInHex[worker]));
}

void UManageHex::RemoveWorkers(WorkerType worker, int workers)
{
	controller->selectedHex->workersInHex[worker] -= UnitActions::RemoveWorkers(controller->playerFaction, worker, workers, controller->selectedHex->workersInHex[worker]);
	controller->selectedHex->ActiveHarvesting();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("%d workers in hex"), controller->selectedHex->workersInHex[worker]));
}
