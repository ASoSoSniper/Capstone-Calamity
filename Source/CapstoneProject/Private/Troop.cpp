// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "UnitActions.h"

void ATroop::CreatePath()
{
	bool hostilityTriggered = false;

	TArray<AActor*> objectsInHex = Cast<ABaseHex>(hexNav->targetHex)->GetObjectsInHex();
	for (int i = 0; i < objectsInHex.Num(); ++i)
	{
		if (UnitActions::IsHostileTarget(this, objectsInHex[i]))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Targeting Enemy"));
			hostilityTriggered = true;
			break;
		}
	}

	attacking = hostilityTriggered;

	Super::CreatePath();
}

void ATroop::MergeOnTile()
{
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
