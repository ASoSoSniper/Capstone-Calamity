// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerPlant.h"
#include "GlobalSpawner.h"

APowerPlant::APowerPlant()
{
	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/EnergyStationMat01"));
	if (visibleMat)
	{
		visibility->meshMaterials.visibleTexture = visibleMat;
	}
	UMaterialInterface* hiddenMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/EnergyStationMat01_Hidden"));
	if (hiddenMat)
	{
		visibility->meshMaterials.hiddenTexture = hiddenMat;
	}
	UMaterialInterface* selectedMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/EnergyStationMat01_HL"));
	if (selectedMat)
	{
		visibility->meshMaterials.selectedTexture = selectedMat;
	}

	buildingType = SpawnableBuildings::PowerPlant;
}

void APowerPlant::SetToFinishedModel()
{
	UStaticMesh* meshAsset = nullptr;

	if (unitStats->faction == Factions::Human)
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingEnergyStationPlains.BuildingEnergyStationPlains'"));
	}
	else
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingPowerPlantAlien.BuildingPowerPlantAlien'"));

		UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/AlienEnergyStationMat01"));
		if (visibleMat)
		{
			visibility->meshMaterials.visibleTexture = visibleMat;
		}
	}

	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
}
