// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerPlant.h"

APowerPlant::APowerPlant()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingEnergyStationPlains.BuildingEnergyStationPlains'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
}

void APowerPlant::UpdateResources()
{
	Cast<ABaseHex>(hexNav->currentHex)->UpdateEnergyYield(energyYield);
}
