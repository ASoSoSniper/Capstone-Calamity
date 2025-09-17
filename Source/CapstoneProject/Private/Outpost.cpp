// Fill out your copyright notice in the Description page of Project Settings.


#include "Outpost.h"
#include "MergedArmy.h"
#include "Troop.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"
#include "TroopFactory.h"

AOutpost::AOutpost()
{
	UMaterialInterface* visibleMat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/OutpostMat01"));
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

	buildingType = SpawnableBuildings::Outpost;
	range = 3;
}

void AOutpost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TSet<ABaseHex*> AOutpost::ClaimLand()
{
	TSet<ABaseHex*> hexesToClaim;

	Factions faction = GetUnitData()->GetFaction();

	if (faction == Factions::None)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("No faction assigned, cannot claim land"));
		return hexesToClaim;
	}

	hexesToClaim = hexNav->GetCurrentHex()->GetHexesInRadius(range);
	for (ABaseHex* hex : hexesToClaim)
	{
		hex->SetHexOwner(faction);
	}
	
	return hexesToClaim;
}

void AOutpost::BuildingAction()
{
	claimedHexes = ClaimLand();
}

void AOutpost::SetToFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingOutpost.BuildingOutpost'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
		mesh->SetRelativeRotation(FRotator(0, -90.f, 0));
	}
}

void AOutpost::Action1()
{
	
}

void AOutpost::Action2()
{
	
}

void AOutpost::Action3()
{
}

void AOutpost::Action4()
{
}

void AOutpost::Destroyed()
{
	for (ABaseHex* hex : claimedHexes)
	{
		hex->SetHexOwner(Factions::None);
	}

	Super::Destroyed();
}
