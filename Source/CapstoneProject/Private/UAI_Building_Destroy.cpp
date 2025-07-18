// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_Building_Destroy.h"
#include "Building.h"

void UUAI_Building_Destroy::ExecuteAction(IUAI_Controller* controller)
{
	ABuilding* building = Cast<ABuilding>(controller);
	if (!building) return;

	building->BeginDestroying();
}
