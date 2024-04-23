// Fill out your copyright notice in the Description page of Project Settings.


#include "RockCity.h"
#include "CapstoneProjectGameModeBase.h"

ARockCity::ARockCity()
{
	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/AlienRockCityMat01"));
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

	unitStats->currentHP = 100;
	unitStats->maxHP = 100;
	unitStats->damage = 10;

	buildingType = SpawnableBuildings::RockCity;
}

void ARockCity::SetToFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingMeteorCity.BuildingMeteorCity'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}

	ACapstoneProjectGameModeBase::cinematicObjects.Add(this, cinematicComponent->cinematicVars);
	spawner->controller->SetCinematicObject(this);
}

bool ARockCity::IsDisabled()
{
	if (Super::IsDisabled())
	{
		ACapstoneProjectGameModeBase::gameState = GameStates::Victory;
		return true;
	}

	return false;
}
