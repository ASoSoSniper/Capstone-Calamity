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

	if (parentTroop->hexNav->targetHex == nullptr || parentTroop->hexNav->currentHex == parentTroop->hexNav->targetHex)
	{
		SetDestination();
	}
}

void UAITroopComponent::SetDestination()
{
	AActor* targetHex = nullptr;

	targetHex = SelectClosestHostileTarget();

	/*int giveUp = 50;
	while (!targetHex)
	{
		targetHex = FindRandomHex();

		--giveUp;
		if (giveUp <= 0) break;
	}*/

	if (targetHex)
	{
		parentTroop->hexNav->targetHex = targetHex;
		parentTroop->CreatePath();
	}
}

ABaseHex* UAITroopComponent::FindHex(int X, int Y)
{
	if (parentTroop->spawner->hexArray.Num() - 1 > X)
	{
		if (parentTroop->spawner->hexArray[X].Num() - 1 > Y)
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
	for (int i = 0; i < targetList.Num(); i++)
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
		UHexNav* hexNav = targetList[closestIndex]->GetComponentByClass<UHexNav>();
		if (hexNav) 
			return hexNav->currentHex;
	}

	return nullptr;
}

AActor* UAITroopComponent::FindRandomHex()
{
	int randX = FMath::RandRange(0, parentTroop->spawner->hexArray.Num() - 1);
	int randY = FMath::RandRange(0, parentTroop->spawner->hexArray[randX].Num() - 1);
	ABaseHex* randomHex = FindHex(randX, randY);

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



