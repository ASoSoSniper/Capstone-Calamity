// Fill out your copyright notice in the Description page of Project Settings.


#include "MiningStation.h"
#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"

AMiningStation::AMiningStation()
{
	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/ProdStationMat01"));
	if (visibleMat)
	{
		visibility->meshMaterials.visibleTexture = visibleMat;
	}
	UMaterialInterface* hiddenMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/ProdStationMat01_Hidden"));
	if (hiddenMat)
	{
		visibility->meshMaterials.hiddenTexture = hiddenMat;
	}
	UMaterialInterface* selectedMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/ProdStationMat01_HL"));
	if (selectedMat)
	{
		visibility->meshMaterials.selectedTexture = selectedMat;
	}

	buildingType = SpawnableBuildings::MiningStation;
}

void AMiningStation::SetToFinishedModel()
{
	UStaticMesh* meshAsset = nullptr;

	if (unitStats->faction == Factions::Human)
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingMiningStationModel.BuildingMiningStationModel'"));
	}
	else
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingMiningStationAlien.BuildingMiningStationAlien'"));
		UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/AlienMiningStationMat01"));
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
