// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "UnitActions.h"

void ATroop::CreatePath()
{
	bool hostilityTriggered = false;

	TArray<AActor*> hexesToCheck = Cast<ABaseHex>(hexNav->targetHex)->GetObjectsInHex();
	for (int i = 0; i < hexesToCheck.Num(); ++i)
	{
		if (UnitActions::IsHostileTarget(this, hexesToCheck[i]))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Targeting Enemy"));
			hostilityTriggered = true;
			break;
		}
	}

	attacking = hostilityTriggered;

	Super::CreatePath();
}
