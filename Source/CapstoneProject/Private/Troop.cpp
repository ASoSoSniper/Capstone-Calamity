// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "MergedArmy.h"
#include "UnitActions.h"
#include "Kismet/GameplayStatics.h"

ATroop::ATroop()
{
	
}

void ATroop::BeginPlay()
{
	Super::BeginPlay();

	UnitActions::AssignFaction(unitStats->faction, this);
	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}
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
	Super::CreatePath();
}


void ATroop::MergeOnTile()
{
	if (hexNav->currentHex == targetToMerge->hexNav->currentHex)
	{
		spawner->MergeArmies(this, targetToMerge, Cast<ABaseHex>(hexNav->currentHex));
	}
}

void ATroop::InputUnitStats(UnitActions::UnitData data)
{
	unitStats->faction = data.faction;
	unitStats->HP_current = data.currentHP;
	unitStats->HP_max = data.maxHP;
	unitStats->defense = data.defense;
	unitStats->speed = data.speed;
	unitStats->currentMorale = data.currentMorale;
	unitStats->maxMorale = data.maxMorale;
	unitStats->minDamage = data.minDamage;
	unitStats->maxDamage = data.maxDamage;
	unitStats->reinforceRate = data.reinforceRate;
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
