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
	if (hexNav->currentHex == targetToMerge->hexNav->currentHex)
	{
		if (spawner && targetToMerge)
		{
			spawner->MergeArmies(this, targetToMerge, Cast<ABaseHex>(hexNav->currentHex));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No spawner found"));
		}
	}
}

void ATroop::InputUnitStats(UnitActions::UnitData data)
{
	unitStats->faction = data.faction;
	unitStats->HP_current = data.currentHP;
	unitStats->HP_max = data.maxHP;
	unitStats->defense = data.defense;
	unitStats->speed = data.speed;
	unitStats->currentMorale = data.currentMorale;
	unitStats->maxMorale = data.maxMorale;
	unitStats->minDamage = data.minDamage;
	unitStats->maxDamage = data.maxDamage;
	unitStats->reinforceRate = data.reinforceRate;
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

	if (hexPathIndex < hexPath.Num())
		RotateToFaceTarget(hexPath[hexPathIndex]->GetActorLocation() - hexNav->currentHex->GetActorLocation(), DeltaTime);

	if (EndForVSlice && Cast<ABaseHex>(hexNav->currentHex)->hexTerrain == TerrainType::TheRock)
	{
		ACapstoneProjectGameModeBase::end = true;
	}
}

