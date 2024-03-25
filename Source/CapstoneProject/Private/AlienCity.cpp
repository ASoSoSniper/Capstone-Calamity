// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienCity.h"
#include "CityEmbassy.h"
#include "CityPoliceStation.h"
#include "CityTradeOutpost.h"
#include "BuildingAttachment.h"
#include "OutpostStorage.h"
#include "OutpostBarracks.h"
#include "OutpostTroopFactory.h"
#include "OutpostDefenses.h"

AAlienCity::AAlienCity()
{
	cityEmbassy = CreateDefaultSubobject<UCityEmbassy>(TEXT("Embassy"));
	cityPoliceStation = CreateDefaultSubobject<UCityPoliceStation>(TEXT("Police Station"));
	cityTradeOutpost = CreateDefaultSubobject<UCityTradeOutpost>(TEXT("Trade Outpost"));

	builtAtStart = true;

	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/AlienNormalCityMat01"));
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

	buildingType = SpawnableBuildings::AlienCity;
}

UBuildingAttachment* AAlienCity::GetAttachment(BuildingAttachments attachment)
{
	if (UBuildingAttachment* attachmentType = Super::GetAttachment(attachment))
	{
		return attachmentType;
	}

	switch (attachment)
	{
	case BuildingAttachments::Embassy:
		return cityEmbassy;
	case BuildingAttachments::PoliceStation:
		return cityPoliceStation;
	case BuildingAttachments::TradeOutpost:
		return cityTradeOutpost;
	default:
		return nullptr;
	}
}

void AAlienCity::SetToFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingAlienCity.BuildingAlienCity'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
}
