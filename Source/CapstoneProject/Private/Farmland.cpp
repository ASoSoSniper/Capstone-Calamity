// Fill out your copyright notice in the Description page of Project Settings.


#include "Farmland.h"
#include "GlobalSpawner.h"

AFarmland::AFarmland()
{
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

void AFarmland::UpdateResources()
{
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
	if (!hex) return;

	if (producingFood)
		hex->UpdateResourceYield(StratResources::Food, resourceYields[StratResources::Food]);
	else
		hex->UpdateResourceYield(StratResources::Wealth, resourceYields[StratResources::Wealth]);
}

void AFarmland::RevertResources()
{
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
	if (!hex) return;

	if (producingFood)
		hex->UpdateResourceYield(StratResources::Food, -resourceYields[StratResources::Food]);
	else
		hex->UpdateResourceYield(StratResources::Wealth, -resourceYields[StratResources::Wealth]);
}

void AFarmland::SetToFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingFarmingStationModel.BuildingFarmingStationModel'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
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
