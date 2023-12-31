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
	subSelect = None;
}



void UManageHex::Action1()
{
	switch (subSelect)
	{
	case None:
		subSelect = Build;
		break;
	case Build:
		if (controller->selectedHex->hexOwner == controller->playerFaction)
			controller->Build(SpawnableBuildings::Farmland);
		Reset();
		break;
	}
}

void UManageHex::Action2()
{
	switch (subSelect)
	{
	case None:
		
		break;
	case Build:
		if (controller->selectedHex->hexOwner == controller->playerFaction)
			controller->Build(SpawnableBuildings::MiningStation);
		Reset();
		break;
	}
}

void UManageHex::Action3()
{
	switch (subSelect)
	{
	case None:
		if (controller->selectedHex->hexOwner == controller->playerFaction)
			AddWorkersToHex(WorkerType::Human, 2);
		break;
	case Build:
		if (controller->selectedHex->hexOwner == controller->playerFaction)
			controller->Build(SpawnableBuildings::PowerPlant);
		Reset();
		break;
	}
}

void UManageHex::Action4()
{
	switch (subSelect)
	{
	case None:
		if (controller->selectedHex->hexOwner == controller->playerFaction)
			RemoveWorkers(WorkerType::Human, 2);
		break;
	case Build:
		controller->Build(SpawnableBuildings::Outpost);
		Reset();
		break;
	}
}

void UManageHex::Action5()
{
	switch (subSelect)
	{
	case None:
		
		break;
	case Build:
		//controller->Build(SpawnableBuildings::Storage);
		Reset();
		break;
	}
}

void UManageHex::Action6()
{
	switch (subSelect)
	{
	case None:

		break;
	case Build:
		//controller->Build(SpawnableBuildings::PowerPlant);
		Reset();
		break;
	}
}

void UManageHex::Action7()
{
	switch (subSelect)
	{
	case None:

		break;
	case Build:
		//controller->Build(SpawnableBuildings::PowerPlant);
		Reset();
		break;
	}
}

void UManageHex::Action8()
{
	switch (subSelect)
	{
	case None:

		break;
	case Build:
		//controller->Build(SpawnableBuildings::PowerPlant);
		Reset();
		break;
	}
}

void UManageHex::Action9()
{
	switch (subSelect)
	{
	case None:

		break;
	case Build:
		//controller->Build(SpawnableBuildings::PowerPlant);
		Reset();
		break;
	}
}

void UManageHex::Action10()
{
	switch (subSelect)
	{
	case None:

		break;
	case Build:
		//controller->Build(SpawnableBuildings::PowerPlant);
		Reset();
		break;
	}
}

void UManageHex::Action11()
{
	switch (subSelect)
	{
	case None:

		break;
	case Build:
		//controller->Build(SpawnableBuildings::PowerPlant);
		Reset();
		break;
	}
}

void UManageHex::Action12()
{
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
