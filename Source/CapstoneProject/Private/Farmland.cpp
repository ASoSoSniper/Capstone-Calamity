// Fill out your copyright notice in the Description page of Project Settings.


#include "Farmland.h"

AFarmland::AFarmland()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingFarmingStationModel.BuildingFarmingStationModel'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
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
	Cast<ABaseHex>(hexNav->currentHex)->UpdateFoodYield(foodYield);
}
