// Fill out your copyright notice in the Description page of Project Settings.


#include "MiningStation.h"
#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.h"

AMiningStation::AMiningStation()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingMiningStationModel.BuildingMiningStationModel'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}

	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/ProdStationMat01"));
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
}

void AMiningStation::BeginPlay()
{
	Super::BeginPlay();
}

void AMiningStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMiningStation::Harvest(ABaseHex* hex)
{
	switch (buildState)
	{
	case Complete:
		//UnitActions::HarvestResources(unitStats->faction, productionYield + hex->resourceBonuses[StratResources::Production], StratResources::Production);
		break;
	case Upgraded:
		break;
	}
	
}

void AMiningStation::UpdateResources()
{
	Cast<ABaseHex>(hexNav->currentHex)->UpdateProductionYield(productionYield);
}
