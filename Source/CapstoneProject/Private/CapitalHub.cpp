// Fill out your copyright notice in the Description page of Project Settings.


#include "CapitalHub.h"
#include "CapstoneProjectGameModeBase.h"

ACapitalHub::ACapitalHub()
{
	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/ShipMat01"));
	if (visibleMat)
	{
		visibility->meshMaterials.visibleTexture = visibleMat;
	}
	UMaterialInterface* hiddenMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/ShipMat01_Hidden"));
	if (hiddenMat)
	{
		visibility->meshMaterials.hiddenTexture = hiddenMat;
	}
	UMaterialInterface* selectedMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/ShipMat01_HL"));
	if (selectedMat)
	{
		visibility->meshMaterials.selectedTexture = selectedMat;
	}

	builtAtStart = true;
	buildingType = SpawnableBuildings::Capitol;

	range = 3;
	visibility->visibilityRadius = 80.f;
	visibility->detectionDistanceInRadius = 75.f;

	unitStats->vision = 3;
}

void ACapitalHub::SetToFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingCapitolShip2.BuildingCapitolShip2'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
		mesh->SetRelativeRotation(FRotator(0, -45.f, 0));
	}
	
	ACapstoneProjectGameModeBase::cinematicObjects.Add(this, cinematicComponent->cinematicVars);
	AGlobalSpawner::spawnerObject->controller->SetCinematicObject(this);
	//spawner->SpawnEndParticle(this, GameStates::Victory);
}

void ACapitalHub::Destroyed()
{
	if (AGlobalSpawner::spawnerObject)
	{
		AGlobalSpawner::spawnerObject->controller->SetCinematicObject(this, GameStates::Defeat);
		AGlobalSpawner::spawnerObject->SpawnEndParticle(this, GameStates::Defeat);
	}

	Super::Destroyed();
}
