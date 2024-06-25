// Fill out your copyright notice in the Description page of Project Settings.


#include "HexNav.h"

// Sets default values for this component's properties
UHexNav::UHexNav()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UHexNav::BeginPlay()
{
	AMovementAI* movement = Cast<AMovementAI>(GetOwner());
	movableEntity = movement != nullptr;
}

void UHexNav::SetCurrentHex(AActor* hex)
{
	if (ABaseHex* castHex = Cast<ABaseHex>(hex))
	{
		currentHex = castHex;
	}
}

void UHexNav::SetTargetHex(AActor* hex)
{
	if (!movableEntity) return;

	if (ABaseHex* castHex = Cast<ABaseHex>(hex))
	{
		targetHex = castHex;
	}
}

ABaseHex* UHexNav::GetCurrentHex()
{
	return currentHex;
}

ABaseHex* UHexNav::GetTargetHex()
{
	return targetHex;
}

bool UHexNav::CurrentEqualToTarget()
{
	return currentHex == targetHex;
}

