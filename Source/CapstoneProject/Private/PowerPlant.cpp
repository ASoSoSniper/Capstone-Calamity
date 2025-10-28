// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerPlant.h"
#include "GlobalSpawner.h"

APowerPlant::APowerPlant()
{
	buildingType = SpawnableBuildings::PowerPlant;
}

UStaticMesh* APowerPlant::LoadFinishedModel()
{
	UStaticMesh* meshAsset = nullptr;

	if (unitData->GetFaction() == Factions::Human)
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingEnergyStationPlains.BuildingEnergyStationPlains'"));
	}
	else
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingPowerPlantAlien.BuildingPowerPlantAlien'"));
	}

	return meshAsset;
}
