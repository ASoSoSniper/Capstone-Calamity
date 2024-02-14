// Fill out your copyright notice in the Description page of Project Settings.


#include "Outpost.h"
#include "OutpostStorage.h"
#include "OutpostBarracks.h"
#include "OutpostTroopFactory.h"
#include "BuildingAttachment.h"
#include "MergedArmy.h"
#include "Troop.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"

AOutpost::AOutpost()
{
	storageBuilding = CreateDefaultSubobject<UOutpostStorage>(TEXT("Storage"));
	barracksBuilding = CreateDefaultSubobject<UOutpostBarracks>(TEXT("Barracks"));
	troopFactoryBuilding = CreateDefaultSubobject<UOutpostTroopFactory>(TEXT("Troop Factory"));

	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingOutpost.BuildingOutpost'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
		mesh->SetRelativeRotation(FRotator(0, -90.f, 0));
	}

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

	visibility->visibilityRadius *= range;
	buildingType = SpawnableBuildings::Outpost;
}

void AOutpost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentTroopBuildTime > 0)
	{
		currentTroopBuildTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
	}
	else
	{
		if (!cuedUnits.IsEmpty() && spawner)
		{
			spawner->BuildTroop(unitStats->faction, cuedUnits[0], Cast<ABaseHex>(hexNav->currentHex));
			cuedUnits.RemoveAt(0);
			if (!cuedUnits.IsEmpty())
			{
				currentTroopBuildTime = spawner->troopCosts[cuedUnits[0]].timeToBuild;
			}
		}
	}
}

TArray<ABaseHex*> AOutpost::ClaimLand()
{
	TArray<ABaseHex*> hexesToClaim;

	if (unitStats->faction == Factions::None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("No faction assigned, cannot claim land"));
		return hexesToClaim;
	}

	ABaseHex* startHex = Cast<ABaseHex>(hexNav->currentHex);

	startHex->hexOwner = unitStats->faction;
	hexesToClaim.Add(startHex);

	for (int i = 0; i < range; ++i)
	{
		TArray<ABaseHex*> currentLayer;

		for (int j = 0; j < hexesToClaim.Num(); ++j)
		{
			TArray<ABaseHex*> tempList;
			tempList = ScanHex(hexesToClaim[j]);
			for (int k = 0; k < tempList.Num(); ++k)
			{
				currentLayer.Add(tempList[k]);
			}
		}

		for (int j = 0; j < currentLayer.Num(); ++j)
		{
			hexesToClaim.Add(currentLayer[j]);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("%d hexes claimed"), hexesToClaim.Num()));

	return hexesToClaim;
}

TArray<ABaseHex*> AOutpost::ScanHex(ABaseHex* hex)
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(hex);
	TArray<AActor*> objectsInHex = Cast<ABaseHex>(hex)->GetObjectsInHex();
	if (objectsInHex.Num() > 0) queryParams.AddIgnoredActors(objectsInHex);

	TArray<ABaseHex*> foundHexes;

	for (int i = 0; i < 6; ++i)
	{
		//Establish direction
		FVector traceStart = hex->GetActorLocation() + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceStartOffset;
		FVector traceEnd = traceStart + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceLength;
		FHitResult hit;

		//Trace
		bool found = GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams, FCollisionResponseParams::DefaultResponseParam);

		//Check for hex actor class
		ABaseHex* foundHex = found ? Cast<ABaseHex>(hit.GetActor()) : nullptr;

		//Check found hex for neutrality
		if (foundHex)
		{
			if (foundHex->hexOwner == Factions::None)
			{
				//Add hex to list of found hexes
				foundHex->SetFaction(unitStats->faction);
				foundHexes.Add(foundHex);
			}
		}
	}

	return foundHexes;
}

void AOutpost::BuildingAction()
{
	claimedHexes = ClaimLand();

	//Build these attachments by default for testing purposes
	BuildAttachment(BuildingAttachments::Storage);
	BuildAttachment(BuildingAttachments::Barracks);
	BuildAttachment(BuildingAttachments::RobotFactory);
}

void AOutpost::BuildAttachment(BuildingAttachments attachment)
{	
	switch (attachment)
	{
	case BuildingAttachments::Storage:
		storageBuilding->ActivateAttachment();
		break;
	case BuildingAttachments::Barracks:
		barracksBuilding->ActivateAttachment();
		break;
	case BuildingAttachments::RobotFactory:
		troopFactoryBuilding->ActivateAttachment();
		break;
	case BuildingAttachments::DefenseStation:
		break;
	}

	
}

bool AOutpost::BuildingAttachmentIsActive(BuildingAttachments attachment)
{
	switch (attachment)
	{
	case Storage:
		return storageBuilding->AttachmentIsActive();
	case RobotFactory:
		return troopFactoryBuilding->AttachmentIsActive();
	case Barracks:
		return storageBuilding->AttachmentIsActive();
	default:
		return false;
	}
}

void AOutpost::CueTroopBuild(SpawnableUnits unit)
{
	if (!spawner) return;
	if (!troopFactoryBuilding->AttachmentIsActive())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Robot factory not built"));
		return;
	}

	if (spawner->troopCosts.Contains(unit))
	{
		bool purchased = spawner->PurchaseTroop(unitStats->faction, unit, this);
		if (purchased)
		{
			cuedUnits.Add(unit);
			if (cuedUnits.Num() == 1) currentTroopBuildTime = spawner->troopCosts[unit].timeToBuild;
		}
	}
}

void AOutpost::StoreTroop(ATroop* troop)
{
	UnitActions::UnitData troopData = UnitActions::CollectUnitData(troop->unitStats);
	troopsInStorage.Add(troopData);

	troop->Destroy();
}

TArray<ATroop*> AOutpost::ReleaseTroops()
{
	TArray<ATroop*> spawnedTroops;
	TArray<ABaseHex*> usedHexes;
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);

	for (int i = 0; i < troopsInStorage.Num(); ++i)
	{
		ATroop* spawn = nullptr;
		ABaseHex* spawnPoint = hex->FindFreeAdjacentHex(hex->hexOwner, usedHexes);
		usedHexes.Add(spawnPoint);

		switch (troopsInStorage[i].unitType)
		{
		case UnitTypes::Army:
			spawn = spawner->SpawnArmy(spawnPoint, troopsInStorage[i].savedUnits);
			break;

		default:
			spawn = spawner->SpawnTroop(spawnPoint, troopsInStorage[i]);
			break;
		}

		spawnedTroops.Add(spawn);
	}
	troopsInStorage.Empty();
	return spawnedTroops;
}

void AOutpost::Action1()
{
	CueTroopBuild(SpawnableUnits::Infantry);
}

void AOutpost::Action2()
{
	ReleaseTroops();
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

	Super::Destroyed();
}
