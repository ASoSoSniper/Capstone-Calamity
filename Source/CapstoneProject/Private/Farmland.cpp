// Fill out your copyright notice in the Description page of Project Settings.


#include "Farmland.h"
#include "GlobalSpawner.h"

AFarmland::AFarmland()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingFarmingStationModel.BuildingFarmingStationModel'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}

	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/FoodStationMat02"));
	if (visibleMat)
	{
		visibility->meshMaterials.visibleTexture = visibleMat;
	}
	UMaterialInterface* hiddenMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/FoodStationMat01_Hidden"));
	if (hiddenMat)
	{
		visibility->meshMaterials.hiddenTexture = hiddenMat;
	}
	UMaterialInterface* selectedMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/FoodStationMat01_HL"));
	if (selectedMat)
	{
		visibility->meshMaterials.selectedTexture = selectedMat;
	}

	buildingType = SpawnableBuildings::Farmland;
}

void AFarmland::Harvest(ABaseHex* hex)
{
	switch (buildState)
	{
	case Complete:
		//UnitActions::HarvestResources(unitStats->faction, foodYield + hex->resourceBonuses[StratResources::Food], StratResources::Food);
		break;
	case Upgraded:
		break;
	}
}

void AFarmland::UpdateResources()
{
	if (producingFood)
		Cast<ABaseHex>(hexNav->currentHex)->UpdateFoodYield(resourceYield);
	else
		Cast<ABaseHex>(hexNav->currentHex)->UpdateWealthYield(resourceYield);
}

void AFarmland::RevertResources()
{
	if (producingFood)
		Cast<ABaseHex>(hexNav->currentHex)->UpdateFoodYield(-resourceYield);
	else
		Cast<ABaseHex>(hexNav->currentHex)->UpdateWealthYield(-resourceYield);
}

bool AFarmland::ToggleResourcesProduced(bool produceFood)
{
	if (producingFood == produceFood) return producingFood;

	producingFood = produceFood;

	RevertResources();
	UpdateResources();
	return producingFood;
}
