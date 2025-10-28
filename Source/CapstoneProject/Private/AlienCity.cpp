// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienCity.h"

AAlienCity::AAlienCity()
{
	builtAtStart = true;
	preAssignedFaction = Factions::Alien1;

	buildingType = SpawnableBuildings::AlienCity;
}

UStaticMesh* AAlienCity::LoadFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingAlienCity.BuildingAlienCity'"));
	
	return meshAsset;
}
