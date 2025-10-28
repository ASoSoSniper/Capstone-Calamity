// Fill out your copyright notice in the Description page of Project Settings.


#include "Farmland.h"
#include "GlobalSpawner.h"

AFarmland::AFarmland()
{
	buildingType = SpawnableBuildings::Farmland;
}

void AFarmland::UpdateResources()
{
	ABaseHex* hex = hexNav->GetCurrentHex();
	if (!hex) return;

	if (producingFood)
		hex->UpdateResourceYield(EStratResources::Food, resourceYields[EStratResources::Food]);
	else
		hex->UpdateResourceYield(EStratResources::Wealth, resourceYields[EStratResources::Wealth]);
}

void AFarmland::RevertResources()
{
	ABaseHex* hex = hexNav->GetCurrentHex();
	if (!hex) return;

	if (producingFood)
		hex->UpdateResourceYield(EStratResources::Food, -resourceYields[EStratResources::Food]);
	else
		hex->UpdateResourceYield(EStratResources::Wealth, -resourceYields[EStratResources::Wealth]);
}

UStaticMesh* AFarmland:: LoadFinishedModel()
{
	UStaticMesh* meshAsset = nullptr;
	if (unitData->GetFaction() == Factions::Human)
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingFarmingStationModel.BuildingFarmingStationModel'"));
	}
	else
	{
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingFarmhouseAlien.BuildingFarmhouseAlien'"));
	}
	
	return meshAsset;
}

bool AFarmland::ToggleResourcesProduced(bool produceFood)
{
	//If already set to desired value, do nothing
	if (producingFood == produceFood) return producingFood;

	//Remove hex yield for the previous resource
	RevertResources();

	//Switch to the new resource
	producingFood = produceFood;
	
	//Add hex yield for new resource
	UpdateResources();

	return producingFood;
}
