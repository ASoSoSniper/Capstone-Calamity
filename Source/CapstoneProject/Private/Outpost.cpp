// Fill out your copyright notice in the Description page of Project Settings.


#include "Outpost.h"
#include "OutpostStorage.h"
#include "OutpostBarracks.h"
#include "OutpostTroopFactory.h"
#include "OutpostDefenses.h"
#include "BuildingAttachment.h"
#include "MergedArmy.h"
#include "Troop.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"
#include "TroopFactory.h"

AOutpost::AOutpost()
{
	storageBuilding = CreateDefaultSubobject<UOutpostStorage>(TEXT("Storage"));
	barracksBuilding = CreateDefaultSubobject<UOutpostBarracks>(TEXT("Barracks"));
	troopFactoryBuilding = CreateDefaultSubobject<UOutpostTroopFactory>(TEXT("Troop Factory"));
	defenseBuilding = CreateDefaultSubobject<UOutpostDefenses>(TEXT("Defense Station"));

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

UBuildingAttachment* AOutpost::GetAttachment(BuildingAttachments attachment)
{
	switch (attachment)
	{
	case BuildingAttachments::Storage:
		return storageBuilding;
	case BuildingAttachments::RobotFactory:
		return troopFactoryBuilding;
	case BuildingAttachments::RobotBarracks:
		return barracksBuilding;
	case BuildingAttachments::DefenseStation:
		return defenseBuilding;
	default:
		return nullptr;
	}
}

void AOutpost::BuildAttachment(BuildingAttachments attachment)
{	
	UBuildingAttachment* attachmentType = GetAttachment(attachment);
	if (attachmentType) attachmentType->ActivateAttachment();
}

void AOutpost::AddWorkersToAttachment(BuildingAttachments attachment, WorkerType worker, int value)
{
	UBuildingAttachment* attachmentType = GetAttachment(attachment);
	if (attachmentType) attachmentType->workersInAttachment[worker] += value;
}

bool AOutpost::BuildingAttachmentIsActive(BuildingAttachments attachment)
{
	UBuildingAttachment* attachmentType = GetAttachment(attachment);
	if (attachmentType) return attachmentType->AttachmentIsActive();

	return false;
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
	TArray<UActorComponent*> attachments;
	GetComponents(UBuildingAttachment::StaticClass(), attachments);

	for (UActorComponent* attachment : attachments)
	{
		if (UBuildingAttachment* buildingAttachment = Cast<UBuildingAttachment>(attachment))
		{
			buildingAttachment->DisableAttachment();
		}
	}
	for (ABaseHex* hex : claimedHexes)
	{
		hex->SetHexOwner(Factions::None);
	}

	Super::Destroyed();
}
