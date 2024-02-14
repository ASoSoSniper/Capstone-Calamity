// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "MergedArmy.h"
#include "UnitActions.h"
#include "Kismet/GameplayStatics.h"
#include "CapstoneProjectGameModeBase.h"

ATroop::ATroop()
{
	
}

void ATroop::BeginPlay()
{
	Super::BeginPlay();

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}
}

void ATroop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!spawnCheck)
	{
		if (unitStats->faction != Factions::None)
		{
			UnitActions::RobotIsActive(unitStats->faction, this);
			spawnCheck = true;
		}
	}

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}

	if (merging)
	{
		if (hexNav->currentHex == hexNav->targetHex)
		{
			MergeOnTile();
			merging = false;
		}
	}
}

void ATroop::CreatePath()
{
	Super::CreatePath();
}


void ATroop::MergeOnTile()
{
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(targetToMerge);
	switch (objectType.type)
	{
	case ObjectTypes::Hex:
		break;
	case ObjectTypes::MoveAI:
		if (hexNav->currentHex == objectType.moveAI->hexNav->currentHex)
		{
			if (spawner && targetToMerge)
			{
				spawner->MergeArmies(this, objectType.moveAI, Cast<ABaseHex>(hexNav->currentHex));
			}
		}
		break;
	case ObjectTypes::Building:
		if (hexNav->currentHex == objectType.building->hexNav->currentHex && objectType.building->unitStats->faction == unitStats->faction)
		{
			if (AOutpost* outpost = Cast<AOutpost>(objectType.building))
			{
				if (!outpost->BuildingAttachmentIsActive(AOutpost::Storage)) return;

				outpost->StoreTroop(this);
			}
		}
	}
	
}

void ATroop::Action1()
{
}

void ATroop::Action2()
{
}

void ATroop::Action3()
{
}

void ATroop::Action4()
{
}

void ATroop::RotateToFaceTarget(FVector direction, float& DeltaTime)
{
	if (!mesh) return;
	FRotator currentRotation = mesh->GetForwardVector().Rotation();
	FRotator targetRotation = direction.Rotation();
	float rotationProgress = 1.f;
	FRotator newRotation = FMath::Lerp(currentRotation, targetRotation, rotationProgress * DeltaTime * rotateSpeedMultiplier * ACapstoneProjectGameModeBase::timeScale);

	mesh->SetWorldRotation(newRotation);
}

void ATroop::MoveToTarget(float& DeltaTime)
{
	Super::MoveToTarget(DeltaTime);

	if (!hexNav->currentHex) return;

	if (hexPathIndex < hexPath.Num())
		RotateToFaceTarget(hexPath[hexPathIndex]->GetActorLocation() - hexNav->currentHex->GetActorLocation(), DeltaTime);

	if (EndForVSlice && Cast<ABaseHex>(hexNav->currentHex)->hexTerrain == TerrainType::TheRock)
	{
		ACapstoneProjectGameModeBase::end = true;
	}
}

