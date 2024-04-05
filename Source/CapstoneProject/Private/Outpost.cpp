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
			BuildTroop();

			cuedUnits.RemoveAt(0);
			if (!cuedUnits.IsEmpty())
			{
				currentTroopBuildTime = spawner->troopCosts[cuedUnits[0]].timeToBuild;
			}
		}
	}

	if (BuildingAttachmentIsActive(BuildingAttachments::RobotBarracks))
	{
		HealTroops(DeltaTime);
	}
}

TArray<ABaseHex*> AOutpost::ClaimLand()
{
	TArray<ABaseHex*> hexesToClaim;

	if (unitStats->faction == Factions::None)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("No faction assigned, cannot claim land"));
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

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("%d hexes claimed"), hexesToClaim.Num()));

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

void AOutpost::CueTroopBuild(UnitTypes unit)
{
	if (!spawner) return;
	if (!troopFactoryBuilding->AttachmentIsActive())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Robot factory not built"));
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

void AOutpost::BuildTroop()
{
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);

	ATroop* troop = spawner->BuildTroop(unitStats->faction, cuedUnits[0], hex);

	if (UnitActions::HexHasFriendlyTroop(unitStats->faction, hex))
	{
		for (int i = 0; i < hex->troopsInHex.Num(); i++)
		{
			if (hex->troopsInHex[i]->unitStats->faction == unitStats->faction)
			{
				if (hex->troopsInHex[i]->unitStats->savedUnits.Num() < Cast<ATroop>(hex->troopsInHex[i])->armyCap)
				{
					troop->SphereCheck(20.f);
					UnitActions::CommandTroopToMerge(troop, hex->troopsInHex[i]);
				}
				else
				{
					TArray<ABaseHex*> ignoredHexes;
					ABaseHex* freeHex = hex->FindFreeAdjacentHex(unitStats->faction, ignoredHexes);

					troop->SetActorLocation(freeHex->troopAnchor->GetComponentLocation());
				}

				break;
			}
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

void AOutpost::HealTroops(float& DeltaTime)
{
	if (currentHealRate > 0)
	{
		currentHealRate -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		return;
	}

	currentHealRate = healRate;
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);

	if (hex->troopsInHex.IsEmpty()) return;

	for (int i = 0; i < hex->troopsInHex.Num(); i++)
	{
		hex->troopsInHex[i]->unitStats->Heal(healAmount);
	}
}

void AOutpost::Action1()
{
	CueTroopBuild(UnitTypes::Infantry);
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
	for (int i = 0; i < claimedHexes.Num(); i++)
	{
		claimedHexes[i]->SetFaction(Factions::None);
	}
	Super::Destroyed();
}
