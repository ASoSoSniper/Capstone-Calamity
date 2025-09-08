// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "MergedArmy.h"
#include "TroopStorage.h"
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

	if (preAssignedFaction != Factions::None)
	{
		if (preAssignedUnitType == UnitTypes::None)
			preAssignedUnitType = UnitTypes::Infantry;

		InitTroop(preAssignedFaction, preAssignedUnitType);
	}
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
		if (hexNav->GetCurrentHex() == objectType.building->hexNav->GetCurrentHex() && objectType.building->GetUnitData()->GetFaction() == unitData->GetFaction())
		{
			if (objectType.building->GetBuildingType() == SpawnableBuildings::RobotBarracks)
			{
				if (ATroopStorage* storage = Cast<ATroopStorage>(objectType.building))
				{
					storage->StoreTroop(this);
				}
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

	data.healthPercent = unitData->GetHPAlpha();
	data.moralePercent = unitData->GetMoraleAlpha();

	data.progressToMove = (float)currTimeTillHexMove / (float)unitData->GetSpeed();

	return data;
}

bool ATroop::CommandTroopToMerge(AActor* target)
{
	UHexNav* targetHexNav = target->GetComponentByClass<UHexNav>();
	UUnitStats* targetStats = target->GetComponentByClass<UUnitStats>();

	//if (unitStats->savedUnits.Num() + targetStats->savedUnits.Num() > armyCap) return false;

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
	float remainingTime = unitData->GetSpeed() - currTimeTillHexMove;

	return remainingTime / hexNav->GetCurrentHex()->GetMovementMulti();
}

void ATroop::InitTroop(const Factions& faction, const UnitTypes& unitType)
{
	using GameMode = ACapstoneProjectGameModeBase;
	if (!GameMode::activeFactions.Contains(faction)) return;

	unitData = new FUnitData(faction, unitType);
	InitTroop(unitData);
}

void ATroop::InitTroop(FUnitData* data)
{
	unitData = data;

	using GameMode = ACapstoneProjectGameModeBase;
	Factions faction = unitData->GetFaction();

	if (GameMode::activeFactions.Contains(faction))
		GameMode::activeFactions[faction]->allUnits.Add(this);

	visibility->SetupComponent(unitData);
}

bool ATroop::SetUpTroop()
{
	//Various checks to ensure all necessary data exists
	if (!AGlobalSpawner::spawnerObject)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No global spawner, could not set up troop"));
		return false;
	}
	if (!unitData || unitData->GetFaction() == Factions::None || unitData->GetUnitType() == UnitTypes::None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Faction or UnitType not set, could not set up troop"));
		return false;
	}

	UnitTypes unitType = unitData->GetUnitType();
	Factions faction = unitData->GetFaction();

	if (!AGlobalSpawner::spawnerObject->troopStats.Contains(unitType) && unitType != UnitTypes::Army)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Troop stats missing, could not set up troop"));
		return false;
	}
	else if (!AGlobalSpawner::spawnerObject->troopFactionMaterials.Contains(faction))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Faction materials missing, could not set up troop"));
		return false;
	}

	//If all data is present, perform setup
	UnitActions::RobotIsActive(faction, this);
	if (unitType != UnitTypes::Army && !unitData->SetupComplete())
	{
		FTroopStats* stats = &AGlobalSpawner::spawnerObject->troopStats[unitType];
		unitData->SetUnitValues(stats->HP, stats->morale, stats->vision, stats->speed, stats->damage, stats->siegePower, stats->reinforceRate, stats->energyUpkeepCost);
	}

	if (faction != Factions::Human)
	{
		if (!debug) AITroopComponent->EnableEnemyAI();
	}

	visibility->meshMaterials.visibleTexture = AGlobalSpawner::spawnerObject->troopFactionMaterials[faction].visibleTexture;
	visibility->meshMaterials.selectedTexture = AGlobalSpawner::spawnerObject->troopFactionMaterials[faction].selectedTexture;

	return true;
}

bool ATroop::DestinationReached() const
{
	if (!targetUnit)
		return hexNav->CurrentEqualToTarget();
	else
		return hexNav->GetCurrentHex() == targetUnit->GetCurrentHex();
}

void ATroop::Destroyed()
{
	if (hexNav->GetCurrentHex())
	{
		Factions faction = unitData->GetFaction();

		if (ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits.Contains(this))
		{
			ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits.Remove(this);
		}
		hexNav->GetCurrentHex()->troopsInHex.Remove(this);

		if (selectedByPlayer)
		{
			AActor* controllerTemp = UGameplayStatics::GetActorOfClass(GetWorld(), ABasePlayerController::StaticClass());
			ABasePlayerController* controller = Cast<ABasePlayerController>(controllerTemp);

			if (controller) controller->Deselect();
		}
	}

	Super::Destroyed();
}

void ATroop::AI_SetMovementAction(UAI_Action* action, const ABaseHex* target)
{
	targetUnit = nullptr;

	hexNav->SetTargetHex(target);
	SetBestAction(action);
}

void ATroop::AI_SetMovementAction(UAI_Action* action, UHexNav* target)
{
	targetUnit = target;

	const ABaseHex* targetHex = targetUnit->GetTargetHex();

	hexNav->SetTargetHex(targetHex);
	SetBestAction(action);
}

void ATroop::UpdateDestination()
{
	const ABaseHex* target = nullptr;

	if (targetUnit)
	{
		if (ATroop* troop = Cast<ATroop>(targetUnit->GetOwner()))
		{
			if (!troop->VisibleToFaction(unitData->GetFaction()))
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
	//targetUnit = nullptr;
	IUAI_Controller::EndAction();
}

UHexNav* ATroop::GetTargetUnit() const
{
	return targetUnit;
}
