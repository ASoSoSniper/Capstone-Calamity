// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienCity.h"

AAlienCity::AAlienCity()
{
	builtAtStart = true;
	preAssignedFaction = Factions::Alien1;

	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/AlienNormalCityMat01"));
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

	buildingType = SpawnableBuildings::AlienCity;
}

void AAlienCity::SetToFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingAlienCity.BuildingAlienCity'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
}
