// Fill out your copyright notice in the Description page of Project Settings.


#include "Outpost.h"
#include "MergedArmy.h"
#include "Troop.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"
#include "TroopFactory.h"

AOutpost::AOutpost()
{
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

UStaticMesh* AOutpost::LoadFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingOutpost.BuildingOutpost'"));

	return meshAsset;
}

void AOutpost::SetToFinishedModel()
{
	Super::SetToFinishedModel();

	if (mesh)
	{
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
