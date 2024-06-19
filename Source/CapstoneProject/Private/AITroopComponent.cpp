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

	if (parentTroop->unitStats->unitType == UnitTypes::Army)
	{
		if (parentTroop->unitStats->savedUnits.IsEmpty()) GenerateArmy();
	}

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
	if (AGlobalSpawner::spawnerObject->hexArray.IsValidIndex(X))
	{
		if (AGlobalSpawner::spawnerObject->hexArray[X].IsValidIndex(Y))
		{
			return AGlobalSpawner::spawnerObject->hexArray[X][Y];
		}
	}

	return nullptr;
}

AActor* UAITroopComponent::SelectClosestHostileTarget()
{
	TMap<ABaseHex*, Factions> targetList = UnitActions::GetTargetList(parentTroop->unitStats->faction);

	if (targetList.IsEmpty()) return nullptr;

	ABaseHex* closestTarget = nullptr;
	float closestDistance = INFINITY;
	for (auto& target : targetList)
	{
		if (!target.Key) continue;

		float distance = FVector::Distance(parentTroop->hexNav->currentHex->GetActorLocation(), target.Key->GetActorLocation());

		if (distance < closestDistance)
		{
			closestDistance = distance;
			closestTarget = target.Key;
		}
	}

	if (closestTarget)
	{
		FVector2D target = AGlobalSpawner::spawnerObject->GetHexCoordinates(Cast<ABaseHex>(closestTarget));
		FVector2D origin = AGlobalSpawner::spawnerObject->GetHexCoordinates(Cast<ABaseHex>(parentTroop->hexNav->currentHex));
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("X Dist: %f, Y Dist: %f"), FMath::Abs(target.X - origin.X), FMath::Abs(target.Y - origin.Y)));
		if (FMath::Abs(target.X - origin.X) > targetAttackDistance ||
			FMath::Abs(target.Y - origin.Y) > targetAttackDistance)
		{
			return nullptr;
		}

		return closestTarget;
	}

	return nullptr;
}

AActor* UAITroopComponent::FindRandomHex()
{
	FVector2D coordinates = AGlobalSpawner::spawnerObject->GetHexCoordinates(Cast<ABaseHex>(parentTroop->hexNav->currentHex));

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
		if (!randomHex->IsTraversableTerrain())
		{
			return nullptr;
		}

		return randomHex;
	}
	return nullptr;
}

void UAITroopComponent::GenerateArmy()
{
	for (int i = 0; i < troopsInArmy; i++)
	{
		UnitTypes randomType = UnitTypes(FMath::RandRange(1, 5));

		UnitActions::UnitData troop = AGlobalSpawner::spawnerObject->CreateTroopUnitData(parentTroop->unitStats->faction, randomType);

		parentTroop->unitStats->savedUnits.Add(troop);
		UnitActions::AddUnitData(parentTroop->unitStats, troop);
	}
}

void UAITroopComponent::EvaluateCurrentTarget()
{
	if (currentTarget != ToEnemy) return;


}



