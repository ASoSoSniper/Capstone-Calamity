// Fill out your copyright notice in the Description page of Project Settings.


#include "AITroopComponent.h"
#include "Troop.h"
#include "BaseHex.h"

// Sets default values for this component's properties
UAITroopComponent::UAITroopComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAITroopComponent::BeginPlay()
{
	Super::BeginPlay();

	parentTroop = Cast<ATroop>(GetOwner());
}


// Called every frame
void UAITroopComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!isEnemy || !parentTroop->hexNav->currentHex) return;

	if (currentTarget != ToEnemy)
	{
		if (AActor* hex = SelectClosestHostileTarget())
		{
			parentTroop->hexNav->targetHex = hex;
			parentTroop->CreatePath();
			currentTarget = ToEnemy;
			return;
		}
	}

	if (parentTroop->hexNav->targetHex == nullptr || parentTroop->hexNav->currentHex == parentTroop->hexNav->targetHex)
	{
		SetDestination();
	}
}

void UAITroopComponent::SetDestination()
{
	AActor* targetHex = nullptr;

	int giveUp = 50;
	while (!targetHex)
	{
		targetHex = FindRandomHex();

		--giveUp;
		if (giveUp <= 0) break;
	}

	if (targetHex)
	{
		parentTroop->hexNav->targetHex = targetHex;
		parentTroop->CreatePath();
		currentTarget = ToHex;
	}
}

ABaseHex* UAITroopComponent::FindHex(int X, int Y)
{
	if (parentTroop->spawner->hexArray.IsValidIndex(X))
	{
		if (parentTroop->spawner->hexArray[X].IsValidIndex(Y))
		{
			return parentTroop->spawner->hexArray[X][Y];
		}
	}

	return nullptr;
}

AActor* UAITroopComponent::SelectClosestHostileTarget()
{
	TArray<AActor*> targetList = UnitActions::GetTargetList(parentTroop->unitStats->faction);

	if (targetList.IsEmpty()) return nullptr;

	int closestIndex = 0;
	float closestDistance = INFINITY;
	for (int i = 0; i < targetList.Num() - 1; i++)
	{
		float distance = FVector::Distance(GetOwner()->GetActorLocation(), targetList[i]->GetActorLocation());

		if (distance < closestDistance)
		{
			closestDistance = distance;
			closestIndex = i;
		}
	}

	if (targetList[closestIndex])
	{
		FVector2D target = parentTroop->spawner->GetHexCoordinates(Cast<ABaseHex>(targetList[closestIndex]));
		FVector2D origin = parentTroop->spawner->GetHexCoordinates(Cast<ABaseHex>(parentTroop->hexNav->currentHex));

		if (FMath::Abs(target.X - origin.X) > targetAttackDistance ||
			FMath::Abs(target.Y - origin.Y) > targetAttackDistance)
		{
			return nullptr;
		}

		UHexNav* hexNav = targetList[closestIndex]->GetComponentByClass<UHexNav>();
		if (hexNav) 
			return hexNav->currentHex;
	}

	return nullptr;
}

AActor* UAITroopComponent::FindRandomHex()
{
	FVector2D coordinates = parentTroop->spawner->GetHexCoordinates(Cast<ABaseHex>(parentTroop->hexNav->currentHex));

	FVector2D hexCoords = coordinates;

	ABaseHex* randomHex = nullptr;
	while (hexCoords == coordinates && !randomHex)
	{
		hexCoords.X = FMath::RandRange(coordinates.X - randomHexInterval, coordinates.X + randomHexInterval);
		hexCoords.Y = FMath::RandRange(coordinates.Y - randomHexInterval, coordinates.Y + randomHexInterval);
		randomHex = FindHex(hexCoords.X, hexCoords.Y);
	}	

	if (randomHex)
	{
		if (!UnitActions::HexIsTraversable(randomHex))
		{
			return nullptr;
		}

		return randomHex;
	}
	return nullptr;
}



