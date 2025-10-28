// Fill out your copyright notice in the Description page of Project Settings.


#include "CapitalHub.h"
#include "CapstoneProjectGameModeBase.h"

ACapitalHub::ACapitalHub()
{
	builtAtStart = true;
	buildingType = SpawnableBuildings::Capitol;

	range = 3;
}

UStaticMesh* ACapitalHub::LoadFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingCapitolShip2.BuildingCapitolShip2'"));

	return meshAsset;
}

void ACapitalHub::SetToFinishedModel()
{
	Super::SetToFinishedModel();

	mesh->SetRelativeRotation(FRotator(0, -45.f, 0));
	
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
