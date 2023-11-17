// Fill out your copyright notice in the Description page of Project Settings.


#include "Troop.h"
#include "MergedArmy.h"
#include "UnitActions.h"
#include "Kismet/GameplayStatics.h"

ATroop::ATroop()
{
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	UStaticMesh* meshComponent = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Knight_Idle.Knight_Idle'"));
	if (meshComponent)
	{
		mesh->SetStaticMesh(meshComponent);
	}
	mesh->SetCollisionProfileName("NoCollision");
	
	mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	mesh->SetupAttachment(sphere);
}

void ATroop::BeginPlay()
{
	Super::BeginPlay();

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

void ATroop::AnalyzeNextHex()
{
	if (hexPathIndex >= hexPath.Num()) return;

	ABaseHex* nextHex = Cast<ABaseHex>(hexPath[hexPathIndex]);

	//If this hex is visible to humans:
	if (nextHex->factionVisibility[Factions::Human].status == ABaseHex::Visible)
	{
		switch (unitStats->faction)
		{
			//If the troop is human, change nothing
		case Factions::Human:
			break;
			//If the troop is alien, set visibility to active
		default:
			ToggleVisibility(false);
			break;
		}
	}
	else
	{
		switch (unitStats->faction)
		{
			//If the troop is human, change nothing
		case Factions::Human:
			break;
			//If the troop is alien, set visibility to active
		default:
			ToggleVisibility(true);
			break;
		}
	}
}

void ATroop::ToggleVisibility(bool visible)
{
	
}
