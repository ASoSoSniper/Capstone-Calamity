// Fill out your copyright notice in the Description page of Project Settings.


#include "MiningStation.h"
#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"

AMiningStation::AMiningStation()
{
	buildingType = SpawnableBuildings::MiningStation;
}

UStaticMesh* AMiningStation::LoadFinishedModel()
{
	UStaticMesh* meshAsset = nullptr;

	if (unitData->GetFaction() == Factions::Human)
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingMiningStationModel.BuildingMiningStationModel'"));
	}
	else
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingMiningStationAlien.BuildingMiningStationAlien'"));
	}

	return meshAsset;
}
