// Fill out your copyright notice in the Description page of Project Settings.


#include "RockCity.h"
#include "CapstoneProjectGameModeBase.h"

ARockCity::ARockCity()
{
	buildingType = SpawnableBuildings::RockCity;
}

UStaticMesh* ARockCity::LoadFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingMeteorCity.BuildingMeteorCity'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
		mesh->EmptyOverrideMaterials();
	}

	ACapstoneProjectGameModeBase::cinematicObjects.Add(this, cinematicComponent->cinematicVars);
	if (AGlobalSpawner::spawnerObject) AGlobalSpawner::spawnerObject->controller->SetCinematicObject(this);

	return meshAsset;
}

bool ARockCity::IsDisabled()
{
	if (Super::IsDisabled())
	{
		if (AGlobalSpawner::spawnerObject)
		{
			AGlobalSpawner::spawnerObject->controller->SetCinematicObject(this, GameStates::Victory);
			AGlobalSpawner::spawnerObject->SpawnEndParticle(this, GameStates::Victory);
		}
		return true;
	}

	return false;
}
