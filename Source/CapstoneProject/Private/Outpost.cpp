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

	buildingType = SpawnableBuildings::Outpost;

	unitStats->vision = 2;
	range = 2;
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
		if (!cuedUnits.IsEmpty() && AGlobalSpawner::spawnerObject)
		{
			BuildTroop();

			cuedUnits.RemoveAt(0);
			if (!cuedUnits.IsEmpty())
			{
				currentTroopBuildTime = AGlobalSpawner::spawnerObject->troopCosts[cuedUnits[0]].timeToBuild;
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

	ABaseHex* startHex = hexNav->GetCurrentHex();

	startHex->SetHexOwner(unitStats->faction);
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
			if (foundHex->GetHexOwner() == Factions::None)
			{
				//Add hex to list of found hexes
				foundHex->SetHexOwner(unitStats->faction);
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

void AOutpost::QueueTroopBuild(UnitTypes unit)
{
	if (!AGlobalSpawner::spawnerObject) return;
	if (!troopFactoryBuilding->AttachmentIsActive()) return;

	//Temp line to remove troop cueing
	//if (!cuedUnits.IsEmpty()) return;

	if (AGlobalSpawner::spawnerObject->troopCosts.Contains(unit))
	{
		bool purchased = AGlobalSpawner::spawnerObject->PurchaseTroop(unitStats->faction, unit, this);
		if (purchased)
		{
			cuedUnits.Add(unit);
			if (cuedUnits.Num() == 1) currentTroopBuildTime = AGlobalSpawner::spawnerObject->troopCosts[unit].timeToBuild;
		}
	}
}

float AOutpost::GetTroopBuildTime()
{
	return currentTroopBuildTime;
}

UnitTypes AOutpost::GetQueuedTroop()
{
	if (cuedUnits.IsEmpty()) return UnitTypes::None;

	return cuedUnits[0];
}

void AOutpost::BuildTroop()
{
	ABaseHex* hex = hexNav->GetCurrentHex();

	ATroop* troop = AGlobalSpawner::spawnerObject->BuildTroop(unitStats->faction, cuedUnits[0], hex);

	AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->troopCompleteSound);

	if (UnitActions::HexHasFriendlyTroop(unitStats->faction, hex, troop))
	{
		for (int i = 0; i < hex->troopsInHex.Num(); i++)
		{
			if (hex->troopsInHex[i]->unitStats->faction == unitStats->faction)
			{
				if (hex->troopsInHex[i]->unitStats->savedUnits.Num() < Cast<ATroop>(hex->troopsInHex[i])->GetArmyCap())
				{
					troop->SphereCheck(20.f);
					troop->CommandTroopToMerge(hex->troopsInHex[i]);
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
	if (!BuildingAttachmentIsActive(BuildingAttachments::Storage)) return;

	UnitActions::UnitData troopData = UnitActions::CollectUnitData(troop->unitStats);
	troopsInStorage.Add(troopData);

	troop->Destroy();
}

TArray<ATroop*> AOutpost::ReleaseTroops()
{
	TArray<ATroop*> spawnedTroops;
	TArray<ABaseHex*> usedHexes;
	ABaseHex* hex = hexNav->GetCurrentHex();

	for (int i = 0; i < troopsInStorage.Num(); ++i)
	{
		ATroop* spawn = nullptr;
		ABaseHex* spawnPoint = hex->FindFreeAdjacentHex(hex->GetHexOwner(), usedHexes);
		usedHexes.Add(spawnPoint);

		switch (troopsInStorage[i].unitType)
		{
		case UnitTypes::Army:
			spawn = AGlobalSpawner::spawnerObject->SpawnArmy(spawnPoint, troopsInStorage[i].savedUnits);
			break;

		default:
			spawn = AGlobalSpawner::spawnerObject->SpawnTroop(spawnPoint, troopsInStorage[i]);
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
	ABaseHex* hex = hexNav->GetCurrentHex();

	if (hex->troopsInHex.IsEmpty()) return;

	for (int i = 0; i < hex->troopsInHex.Num(); i++)
	{
		hex->troopsInHex[i]->unitStats->Heal(healAmount);
	}
}

void AOutpost::Action1()
{
	QueueTroopBuild(UnitTypes::Infantry);
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
		claimedHexes[i]->SetHexOwner(Factions::None);
	}

	Super::Destroyed();
}
