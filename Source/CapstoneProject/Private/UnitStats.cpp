// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitStats.h"
#include "CapstoneProjectGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UUnitStats::UUnitStats()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUnitStats::BeginPlay()
{
	Super::BeginPlay();

	for (auto currentFaction : ACapstoneProjectGameModeBase::activeFactions)
	{
		unitVisibility.Add(currentFaction.Key, false);
	}
	if (faction != Factions::None) unitVisibility[faction] = true;
	currhealTime = maxHealTime;
}


// Called every frame
void UUnitStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HP_current < HP_max)
	{
		currhealTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		if (currhealTime <= 0)
		{
			Heal();
			currhealTime = maxHealTime;
		}
	}

	if (ACapstoneProjectGameModeBase::currentScanTime <= 0.f)
	{
		Visibility(visibilityRadius);
	}
}

bool UUnitStats::IsAlive()
{
	if (HP_current <= 0)
	{
		return false;
	}
	return true;
}

void UUnitStats::Heal()
{
	HP_current += reinforceRate;
	HP_current = FMath::Clamp(HP_current, 0, HP_max);

	if (!savedUnits.IsEmpty()) return;

	for (int i = 0; i < savedUnits.Num(); ++i)
	{
		savedUnits[i].currentHP += reinforceRate;
		savedUnits[i].currentHP = FMath::Clamp(savedUnits[i].currentHP, 0, savedUnits[i].maxHP);
	}
}

void UUnitStats::Visibility(float radius)
{
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(GetOwner());
	TArray<FHitResult> results;

	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), radius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::ForOneFrame, results, true);

	if (bHit)
	{
		for (int i = 0; i < results.Num(); ++i)
		{
			ABaseHex* hex = Cast<ABaseHex>(results[i].GetActor());
			if (hex) hex->ToggleVisibility(faction);
		}
	}
}

