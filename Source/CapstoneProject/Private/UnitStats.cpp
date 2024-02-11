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

	currhealTime = maxHealTime;
}


// Called every frame
void UUnitStats::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (currentHP < maxHP)
	{
		currhealTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		if (currhealTime <= 0)
		{
			//Heal();
			currhealTime = maxHealTime;
		}
	}
}

bool UUnitStats::IsAlive()
{
	if (currentHP <= 0)
	{
		return false;
	}
	return true;
}

void UUnitStats::Heal()
{
	currentHP += reinforceRate;
	currentHP = FMath::Clamp(currentHP, 0, maxHP);

	if (!savedUnits.IsEmpty()) return;

	for (int i = 0; i < savedUnits.Num(); ++i)
	{
		savedUnits[i].currentHP += reinforceRate;
		savedUnits[i].currentHP = FMath::Clamp(savedUnits[i].currentHP, 0, savedUnits[i].maxHP);
	}
}

