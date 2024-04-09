// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshVisibility.h"
#include "HexNav.h"
#include "CapstoneProjectGameModeBase.h"

// Sets default values for this component's properties
UMeshVisibility::UMeshVisibility()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	hexBaseMaterials.hiddenTexture = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/Unclaimed.Unclaimed"));
	hexBaseMaterials.visibleTexture = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/Unclaimed.Unclaimed"));
	hexBaseMaterials.selectedTexture = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/Unclaimed.Unclaimed"));
}


// Called when the game starts
void UMeshVisibility::BeginPlay()
{
	Super::BeginPlay();

	objectType = UnitActions::DetermineObjectType(GetOwner()).type;
	if (objectType == ObjectTypes::MoveAI || objectType == ObjectTypes::Building)
	{
		unitStats = GetOwner()->GetComponentByClass<UUnitStats>();
		hexNav = GetOwner()->GetComponentByClass<UHexNav>();
	}
	else if (objectType == ObjectTypes::Hex)
	{
		enableScan = false;
	}

	mesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	if (!mesh)
	{
		skeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
	}

	FindFactionOfOwner();

	for (auto& curFaction : ACapstoneProjectGameModeBase::activeFactions)
	{
		factionVisibility.Add(curFaction.Key, FVisibility{ VisibilityStatus::Undiscovered, false, false });
	}

	if (faction != Factions::None)
	{
		if (!factionVisibility.Contains(faction)) 
			factionVisibility.Add(faction, FVisibility{ VisibilityStatus::Undiscovered, false, false });
	}
	
}


// Called every frame
void UMeshVisibility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (faction == Factions::None) FindFactionOfOwner();

	if (factionVisibility.Num() < ACapstoneProjectGameModeBase::activeFactions.Num())
	{
		for (auto& curFaction : ACapstoneProjectGameModeBase::activeFactions)
		{
			if (!factionVisibility.Contains(curFaction.Key))
				factionVisibility.Add(curFaction.Key, FVisibility{ VisibilityStatus::Undiscovered, false, false });
		}
		return;
	}

	if (ACapstoneProjectGameModeBase::currentScanTime <= 0.f)
	{
		if (enableScan) Scan(visibilityRadius);
		SetVisibility();
	}
}

void UMeshVisibility::Scan(float radius)
{
	TArray<AActor*> actorsToIgnore;
	if (objectType != ObjectTypes::Hex) actorsToIgnore.Add(GetOwner());
	TArray<FHitResult> results;

	int visionMulti = unitStats ? unitStats->vision : 1;
	float hexVisionMod = (hexNav && hexNav->currentHex) ? Cast<ABaseHex>(hexNav->currentHex)->vision : 0;

	bool zeroVision = ((radius * visionMulti) + (radius * hexVisionMod) == 0);
	float hexMod = zeroVision ? -0.5f : hexVisionMod;

	float searchRadius = (radius * (float)visionMulti) + 
		(radius * hexMod);
	float detectionRadius = (detectionDistanceInRadius * (float)visionMulti) + 
		(radius * hexMod);

	if (debug) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("SearchRadius = %f, visionMod = %f, hexMod = %f"), searchRadius, hexVisionMod, hexMod));

	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), searchRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, showDebugSphere ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, results, true);

	if (bHit)
	{
		for (int i = 0; i < results.Num(); ++i)
		{
			UMeshVisibility* found = results[i].GetActor()->GetComponentByClass<UMeshVisibility>();
			if (found && faction != Factions::None)
			{
				if (FVector::Distance(results[i].GetActor()->GetActorLocation(), GetOwner()->GetActorLocation()) > detectionRadius) continue;

				found->InSight(faction);
			}
		}
	}
}

void UMeshVisibility::InSight(Factions thisFaction)
{
	//Add faction to object if it doesn't already exist
	if (!factionVisibility.Contains(thisFaction))
	{
		factionVisibility.Add(thisFaction, FVisibility{ VisibilityStatus::Undiscovered, false, false });
	}

	//Set object as "In Sight" for the faction and all its allies
	for (auto& ally : ACapstoneProjectGameModeBase::activeFactions)
	{
		if (UnitActions::GetFactionRelationship(thisFaction, ally.Key) == FactionRelationship::Ally)
		{
			factionVisibility[ally.Key].inSight = true;
		}
	}
}

void UMeshVisibility::SetVisibility()
{
	bool visibleToPlayer = false;

	for (auto& curfaction : factionVisibility)
	{
		if (curfaction.Value.inSight)
		{
			factionVisibility[curfaction.Key].discoveredByFaction = true;
			factionVisibility[curfaction.Key].status = VisibilityStatus::Visible;
			factionVisibility[curfaction.Key].inSight = false;

			UnitActions::SetTargetListElement(curfaction.Key, GetOwner(), true);

			if (UnitActions::GetFactionRelationship(Factions::Human, curfaction.Key) == FactionRelationship::Ally) visibleToPlayer = true;
		}
		else
		{
			factionVisibility[curfaction.Key].status = factionVisibility[curfaction.Key].discoveredByFaction ? VisibilityStatus::Discovered : VisibilityStatus::Undiscovered;

			UnitActions::SetTargetListElement(curfaction.Key, GetOwner(), false);
		}
	}

	if (faction == Factions::Human && objectType != ObjectTypes::Hex) visibleToPlayer = true;

	UMaterialInterface* material = nullptr;
	UMaterialInterface* otherMaterial = nullptr;
	UMaterialInterface* baseMaterial = nullptr;

	if (visibleToPlayer)
	{
		switch (objectType)
		{
		case ObjectTypes::Hex:
			if (selected)
			{
				material = meshMaterials.selectedTexture;
				otherMaterial = meshMaterials.modelSelectedTexture;
			}
			else
			{
				material = meshMaterials.visibleTexture;
				otherMaterial = meshMaterials.modelVisibleTexture;
			}

			//if (otherMesh) otherMesh->SetVisibility(true);
			if (hexBaseMesh) baseMaterial = hexBaseMaterials.visibleTexture;

			if (!discoveredByPlayer)
			{
				Cast<ABaseHex>(GetOwner())->RequestTerrainChange(true);
				discoveredByPlayer = true;
			}
			break;

		case ObjectTypes::MoveAI:
			skeletalMesh->SetVisibility(true);

			if (selected)
			{
				material = meshMaterials.selectedTexture;
			}
			else
			{
				material = meshMaterials.visibleTexture;
			}
			break;

		case ObjectTypes::Building:
			if (mesh) mesh->SetVisibility(true);
			if (skeletalMesh) skeletalMesh->SetVisibility(true);

			if (selected)
			{
				material = meshMaterials.selectedTexture;
			}
			else
			{
				material = meshMaterials.visibleTexture;
			}
			break;
		}
	}
	else
	{
		switch (objectType)
		{
		case ObjectTypes::Hex:
			material = meshMaterials.hiddenTexture;
			otherMaterial = meshMaterials.modelHiddenTexture;
			baseMaterial = hexBaseMaterials.hiddenTexture;

			if (!factionVisibility[Factions::Human].discoveredByFaction)
			{
				if (otherMesh) otherMesh->SetVisibility(false);
			}
			break;

		case ObjectTypes::MoveAI:
			if (mesh) mesh->SetVisibility(false);
			if (skeletalMesh) skeletalMesh->SetVisibility(false);
			break;

		case ObjectTypes::Building:
			material = meshMaterials.hiddenTexture;
			if (!factionVisibility[Factions::Human].discoveredByFaction)
			{
				mesh->SetVisibility(false);
			}
			break;
		}
	}

	if (!material)
	{
		return;
	}

	if (mesh)
	{
		if (mesh->GetMaterial(0) != material)
		{
			mesh->SetMaterial(0, material);
			if (debug) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Material changed"));
		}
	}
	if (otherMesh)
	{
		if (otherMesh->GetMaterial(0) != otherMaterial)
		{
			otherMesh->SetMaterial(0, otherMaterial);
			if (debug) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Other material changed"));
		}
	}
	if (hexBaseMesh)
	{
		if (hexBaseMesh->GetMaterial(0) != baseMaterial)
		{
			hexBaseMesh->SetMaterial(0, baseMaterial);
			hexBaseMaterials.hiddenTexture = baseMaterial;
		}
	}
	if (skeletalMesh)
	{
		if (skeletalMesh->GetMaterial(0) != material)
		{
			skeletalMesh->SetMaterial(0, material);
			if (debug) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Material changed"));
		}
	}
	if (otherSkeletalMesh)
	{
		if (otherSkeletalMesh->GetMaterial(0) != material)
		{
			otherSkeletalMesh->SetMaterial(0, material);
			if (debug) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Material changed"));
		}
	}
}

void UMeshVisibility::FindFactionOfOwner()
{
	ABaseHex* hex;
	UUnitStats* unit;
	switch (objectType)
	{
	case ObjectTypes::Hex:
		hex = Cast<ABaseHex>(GetOwner());
		faction = hex->hexOwner;
		otherMesh = hex->hexMeshAttachment;
		break;
	case ObjectTypes::MoveAI:
		unit = GetOwner()->GetComponentByClass<UUnitStats>();
		faction = unit->faction;
		break;
	case ObjectTypes::Building:
		unit = GetOwner()->GetComponentByClass<UUnitStats>();
		faction = unit->faction;
		break;
	}
}

void UMeshVisibility::SetSelected(bool active)
{
	selected = active;
}