// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "MergedArmy.h"
#include "UnitActions.h"

ATroop::ATroop()
{
	mergedArmyPrefab = AMergedArmy::StaticClass();
	troopPrefab = ATroop::StaticClass();
}

void ATroop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (merging)
	{
		if (hexNav->currentHex == hexNav->targetHex)
		{
			MergeOnTile();
			merging = false;
		}
	}
}

void ATroop::CreatePath()
{
	/*bool hostilityTriggered = false;

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

	attacking = hostilityTriggered;*/

	Super::CreatePath();
}


void ATroop::MergeOnTile()
{
	if (hexNav->currentHex == targetToMerge->hexNav->currentHex)
	{
		MergeArmies(this, targetToMerge);
	}
}

void ATroop::MergeArmies(ATroop* seeker, ATroop* target)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Armies merged!"));
	int armies = 0;

	AMergedArmy* testSeeker = Cast<AMergedArmy>(seeker);
	if (testSeeker) armies += 1;
	AMergedArmy* testTarget = Cast<AMergedArmy>(target);
	if (testTarget) armies += 1;

	FActorSpawnParameters params;
	AMergedArmy* mergedArmy;

	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);

	switch (armies)
	{
		//If neither unit is an army, create a new army that consumes both units
	case 0:
		mergedArmy = GetWorld()->SpawnActor<AMergedArmy>(mergedArmyPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);

		hex->troopsInHex.Remove(seeker);
		hex->troopsInHex.Remove(target);
		hex->troopsInHex.Add(mergedArmy);

		mergedArmy->unitStats->faction = seeker->unitStats->faction;
		mergedArmy->ConsumeUnit(seeker);
		mergedArmy->ConsumeUnit(target);
		break;

		//If one unit is an army, determine which unit is that army and merge the other into it
	case 1:
		AMergedArmy* army;
		ATroop* unit;
		if (testSeeker)
		{
			army = testSeeker;
			unit = target;
		}
		else
		{
			army = testTarget;
			unit = seeker;
		}

		hex->troopsInHex.Remove(unit);

		army->ConsumeUnit(unit);
		break;

		//If both units are armies, let the seeker be the dominant army and merge the other army into it
	case 2:
		hex->troopsInHex.Remove(testTarget);

		testSeeker->ConsumeArmy(testTarget);
		break;
	}
}

void ATroop::InputUnitStats(UnitActions::UnitData data)
{
	unitStats->faction = data.faction;
	unitStats->HP_current = data.currentHP;
	unitStats->HP_max = data.maxHP;
	unitStats->defense = data.defense;
	unitStats->speed = data.speed;
	unitStats->minDamage = data.minDamage;
	unitStats->maxDamage = data.maxDamage;
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
