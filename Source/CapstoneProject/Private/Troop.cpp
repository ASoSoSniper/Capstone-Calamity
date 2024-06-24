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
}

void ATroop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!setupComplete)
	{
		setupComplete = SetUpTroop();
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
			if (AGlobalSpawner::spawnerObject && targetToMerge)
			{
				AGlobalSpawner::spawnerObject->MergeArmies(this, objectType.moveAI, Cast<ABaseHex>(hexNav->currentHex));
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

FTroopUIData ATroop::GetUIData()
{
	FTroopUIData data;

	data.healthPercent = (float)unitStats->currentHP / (float)unitStats->maxHP;
	data.moralePercent = (float)unitStats->currentMorale / (float)unitStats->maxMorale;

	data.progressToMove = (float)currTimeTillHexMove / (float)unitStats->speed;

	return data;
}

bool ATroop::SetUpTroop()
{
	//Various checks to ensure all necessary data exists
	if (!AGlobalSpawner::spawnerObject)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No global spawner, could not set up troop"));
		return false;
	}
	else if (unitStats->faction == Factions::None || unitStats->unitType == UnitTypes::None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Faction or UnitType not set, could not set up troop"));
		return false;
	}
	else if (!AGlobalSpawner::spawnerObject->troopStats.Contains(unitStats->unitType) && unitStats->unitType != UnitTypes::Army)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Troop stats missing, could not set up troop"));
		return false;
	}
	else if (!AGlobalSpawner::spawnerObject->troopFactionMaterials.Contains(unitStats->faction))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Faction materials missing, could not set up troop"));
		return false;
	}

	//If all data is present, perform setup
	UnitActions::AssignFaction(unitStats->faction, this);
	UnitActions::RobotIsActive(unitStats->faction, this);
	if (unitStats->unitType != UnitTypes::Army) UnitActions::ApplyDataToUnitStats(unitStats, AGlobalSpawner::spawnerObject->troopStats[unitStats->unitType]);

	if (unitStats->faction != Factions::Human)
	{
		if (!debug) AITroopComponent->EnableEnemyAI();
	}

	visibility->meshMaterials.visibleTexture = AGlobalSpawner::spawnerObject->troopFactionMaterials[unitStats->faction].visibleTexture;
	visibility->meshMaterials.selectedTexture = AGlobalSpawner::spawnerObject->troopFactionMaterials[unitStats->faction].selectedTexture;

	return true;
}

