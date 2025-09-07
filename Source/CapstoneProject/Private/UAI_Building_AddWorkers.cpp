// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_Building_AddWorkers.h"
#include "Building.h"
#include "UnitActions.h"

void UUAI_Building_AddWorkers::ExecuteAction(IUAI_Controller* controller)
{
	ABuilding* building = Cast<ABuilding>(controller);
	if (!building) return;

	ABaseHex* hex = building->hexNav->GetCurrentHex();

	UnitActions::AddWorkers(building->GetUnitData()->GetFaction(), WorkerType::Alien, maxWorkers ? hex->GetMaxWorkers() : workers, hex);
}
