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
		if (hexNav->CurrentEqualToTarget())
		{
			MergeOnTile();
			merging = false;
		}
	}

	FSM_Tick(DeltaTime);
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
		if (hexNav->GetCurrentHex() == objectType.moveAI->hexNav->GetCurrentHex())
		{
			if (AGlobalSpawner::spawnerObject && targetToMerge)
			{
				AGlobalSpawner::spawnerObject->MergeArmies(this, objectType.moveAI, hexNav->GetCurrentHex());
			}
		}
		break;
	case ObjectTypes::Building:
		if (hexNav->GetCurrentHex() == objectType.building->hexNav->GetCurrentHex() && objectType.building->unitStats->faction == unitStats->faction)
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
	if (!hexNav->GetCurrentHex() || hexPath.IsEmpty()) return;

	if (hexPathIndex < hexPath.Num())
		RotateToFaceTarget(hexPath[hexPathIndex]->GetActorLocation() - hexNav->GetCurrentHex()->GetActorLocation(), DeltaTime);

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

bool ATroop::CommandTroopToMerge(AActor* target)
{
	UHexNav* targetHexNav = target->GetComponentByClass<UHexNav>();
	UUnitStats* targetStats = target->GetComponentByClass<UUnitStats>();

	if (unitStats->savedUnits.Num() + targetStats->savedUnits.Num() > armyCap) return false;

	if (targetHexNav->GetCurrentHex())
	{
		merging = true;
		targetToMerge = target;
		SetDestination(targetHexNav->GetCurrentHex());

		return true;
	}

	return false;
}

int ATroop::GetArmyCap()
{
	return armyCap;
}

float ATroop::GetRemainingHexTraversalTime() const
{
	float remainingTime = unitStats->speed - currTimeTillHexMove;

	return remainingTime / hexNav->GetCurrentHex()->GetMovementMulti();
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

bool ATroop::DestinationReached() const
{
	return hexNav->CurrentEqualToTarget();
}

void ATroop::AI_SetMovementAction(UAI_Action* action, const ABaseHex* target)
{
	hexNav->SetTargetHex(target);

	SetBestAction(action);
}

void ATroop::AI_SetMovementAction(UAI_Action* action, UHexNav* target)
{
	targetUnit = target;

	const ABaseHex* targetHex = targetUnit->GetTargetHex();
	AI_SetMovementAction(action, targetHex ? targetHex : targetUnit->GetCurrentHex());
}

void ATroop::UpdateDestination()
{
	const ABaseHex* target = nullptr;

	if (targetUnit)
	{
		if (ATroop* troop = Cast<ATroop>(targetUnit->GetOwner()))
		{
			if (!troop->VisibleToFaction(unitStats->faction))
			{
				EndAction();
				return;
			}
		}

		const ABaseHex* targetUnitDest = targetUnit->GetTargetHex();
		target = targetUnitDest ? targetUnitDest : targetUnit->GetCurrentHex();
	}
	else
	{
		const ABaseHex* unitDest = hexNav->GetTargetHex();
		target = unitDest ? unitDest : hexNav->GetCurrentHex();
	}

	if (target)
	{
		SetDestination(target);
		SetDestinationUpdateTimer(GetRemainingHexTraversalTime());
	}
}

void ATroop::EndAction()
{
	targetUnit = nullptr;
	IUAI_Controller::EndAction();
}

UHexNav* ATroop::GetTargetUnit() const
{
	return targetUnit;
}
