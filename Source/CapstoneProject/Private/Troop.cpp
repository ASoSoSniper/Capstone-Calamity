// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "MergedArmy.h"
#include "UnitActions.h"
#include "Kismet/GameplayStatics.h"
#include "CapstoneProjectGameModeBase.h"

ATroop::ATroop()
{
	AITroopComponent = CreateDefaultSubobject<UAITroopComponent>(TEXT("AI Component"));
}

void ATroop::BeginPlay()
{
	Super::BeginPlay();

	if (unitStats->faction != Factions::Human)
	{
		//AITroopComponent->isEnemy = true;
	}

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
		
		if (spawner && unitStats->unitType != UnitTypes::None)
		{
			if (spawner->troopStats.Contains(unitStats->unitType))
			{
				UnitActions::ApplyDataToUnitStats(unitStats, spawner->troopStats[unitStats->unitType]);
			}
		}
	}

	if (!visibility->meshMaterials.visibleTexture || !visibility->meshMaterials.selectedTexture)
	{
		if (spawner && spawner->troopFactionMaterials.Contains(unitStats->faction))
		{
			visibility->meshMaterials.visibleTexture = spawner->troopFactionMaterials[unitStats->faction].visibleTexture;
			visibility->meshMaterials.selectedTexture = spawner->troopFactionMaterials[unitStats->faction].selectedTexture;
		}
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

	if (unitStats->faction != Factions::Human)
	{
		if (!debug) AITroopComponent->isEnemy = true;
	}

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
		
		if (spawner && unitStats->unitType != UnitTypes::None)
		{
			if (spawner->troopStats.Contains(unitStats->unitType))
			{
				UnitActions::ApplyDataToUnitStats(unitStats, spawner->troopStats[unitStats->unitType]);
			}
		}
	}

	if (!visibility->meshMaterials.visibleTexture || !visibility->meshMaterials.selectedTexture)
	{
		if (spawner && spawner->troopFactionMaterials.Contains(unitStats->faction))
		{
			visibility->meshMaterials.visibleTexture = spawner->troopFactionMaterials[unitStats->faction].visibleTexture;
			visibility->meshMaterials.selectedTexture = spawner->troopFactionMaterials[unitStats->faction].selectedTexture;
		}
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
				if (!outpost->BuildingAttachmentIsActive(BuildingAttachments::Storage)) return;

				outpost->StoreTroop(this);
			}
		}
	}
	
}

void ATroop::Action1()
{
	CancelPath();
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
	if (!hexNav->currentHex) return;

	if (hexPathIndex < hexPath.Num())
		RotateToFaceTarget(hexPath[hexPathIndex]->GetActorLocation() - hexNav->currentHex->GetActorLocation(), DeltaTime);

	Super::MoveToTarget(DeltaTime);
}

