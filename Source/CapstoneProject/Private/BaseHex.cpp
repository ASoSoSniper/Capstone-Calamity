// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHex.h"
#include "MergedArmy.h"
#include "MovementAI.h"
#include "Building.h"
#include "BattleObject.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseHex::ABaseHex()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hexMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hex Mesh"));
	RootComponent = hexMesh;
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/CyTheHexGuy.CyTheHexGuy'"));
	if (meshAsset)
	{
		hexMesh->SetStaticMesh(meshAsset);
	}
	
	//hexInfo = CreateDefaultSubobject<UHexInfo>(TEXT("Hex Info"));
	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));

	collider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	collider->SetupAttachment(RootComponent);
	collider->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	troopAnchor = CreateDefaultSubobject<USceneComponent>("Troop Anchor");
	troopAnchor->SetupAttachment(RootComponent);

	buildingAnchor = CreateDefaultSubobject<USceneComponent>("Building Anchor");
	buildingAnchor->SetupAttachment(RootComponent);

	//Initialize resource yields
	resourceBonuses.Add(StratResources::Energy, ResourceStats{ energyYieldBonus,0 });
	resourceBonuses.Add(StratResources::Production, ResourceStats{ productionYieldBonus, 0 });
	resourceBonuses.Add(StratResources::Food, ResourceStats{ foodYieldBonus, 0 });

	//Initialize worker types
	workersInHex.Add(WorkerType::Human, 0);
	workersInHex.Add(WorkerType::Robot, 0);
	workersInHex.Add(WorkerType::Alien, 0);
}

// Called when the game starts or when spawned
void ABaseHex::BeginPlay()
{
	Super::BeginPlay();
	
	//Add visibility categories for all factions
	for (auto faction : ACapstoneProjectGameModeBase::activeFactions)
	{
		factionVisibility.Add(faction.Key, Visibility{Undiscovered, false, false});
	}

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}

	/*switch (hexTerrain)
	{
	case (TerrainType::Plains):

	}*/

	currentHarvestTime = maxHarvestTime;
}

// Called every frame
void ABaseHex::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ACapstoneProjectGameModeBase::currentScanTime <= 0)
	{
		SetVisibility();
	}
}

TArray<AActor*> ABaseHex::GetObjectsInHex()
{
	TArray<AActor*> actors;

	if (building) 
	{
		actors.Add(building);
	}

	if (troopsInHex.Num() > 0)
	{
		for (int i = 0; i < troopsInHex.Num(); ++i)
		{
			actors.Add(troopsInHex[i]);
		}
	}
	
	return actors;
}

void ABaseHex::CheckForHostility(AMovementAI* refTroop)
{	
	for (int i = 0; i < troopsInHex.Num(); ++i)
	{
		if (troopsInHex[i] != refTroop)
		{
			if (UnitActions::IsHostileTarget(refTroop, troopsInHex[i]))
			{
				BeginBattle();
				return;
			}
		}
	}
}

void ABaseHex::AddTroopToHex(AMovementAI* troop)
{
	troop->hexNav->currentHex = this;
	troopsInHex.Add(troop);

	CheckForHostility(troop);
}

void ABaseHex::RemoveTroopFromHex(AMovementAI* troop)
{
	if (troopsInHex.Contains(troop))
	{
		troopsInHex.Remove(troop);
	}
}

void ABaseHex::BeginBattle()
{
	spawner->SpawnBattle(this);
}

void ABaseHex::Harvest(float& DeltaTime)
{
	if (currentHarvestTime > 0)
	{
		currentHarvestTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		return;
	}

	if (hexOwner != Factions::None)
	{
		UnitActions::HarvestResources(hexOwner, resourceBonuses[StratResources::Food].yieldBonus, StratResources::Food);
		if (building)
		{
			building->Harvest(this);
		}
	}

	currentHarvestTime = maxHarvestTime;
}

bool ABaseHex::ActiveHarvesting()
{
	bool alreadyHarvesting = harvesting;
	bool workersExist = false;

	for (auto workers : workersInHex)
	{
		if (workers.Value >= 10) workersExist = true;
	}

	harvesting = workersExist;

	if (harvesting != alreadyHarvesting)
	{
		ToggleResourceYield();
	}

	return workersExist;
}

void ABaseHex::UpdateFoodYield(int value)
{
	resourceBonuses[StratResources::Food].yieldBonus += value;

	if (!ACapstoneProjectGameModeBase::activeFactions.Contains(hexOwner))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("No faction found")));
		return;
	}

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Food].resourcePerTick += value;
}

void ABaseHex::UpdateProductionYield(int value)
{
	resourceBonuses[StratResources::Production].yieldBonus += value;

	if (!ACapstoneProjectGameModeBase::activeFactions.Contains(hexOwner))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("No faction found")));
		return;
	}

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Production].resourcePerTick += value;
}

void ABaseHex::UpdateEnergyYield(int value)
{
	resourceBonuses[StratResources::Energy].yieldBonus += value;

	if (!ACapstoneProjectGameModeBase::activeFactions.Contains(hexOwner))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("No faction found")));
		return;
	}

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Energy].resourcePerTick += value;
}

void ABaseHex::ToggleResourceYield()
{
	if (hexOwner == Factions::None) return;

	int axis = harvesting ? 1 : -1;

	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Food].resourcePerTick += axis * resourceBonuses[StratResources::Food].yieldBonus;
	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Production].resourcePerTick += axis * resourceBonuses[StratResources::Production].yieldBonus;
	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Energy].resourcePerTick += axis * resourceBonuses[StratResources::Energy].yieldBonus;

	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Food].maxResources += axis * resourceBonuses[StratResources::Food].capBonus;
	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Production].maxResources += axis * resourceBonuses[StratResources::Production].capBonus;
	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Energy].maxResources += axis * resourceBonuses[StratResources::Energy].capBonus;
}

void ABaseHex::UpdateResourceCapIncrease(int value)
{
	resourceBonuses[StratResources::Food].capBonus += value;
	resourceBonuses[StratResources::Production].capBonus += value;
	resourceBonuses[StratResources::Energy].capBonus += value;

	if (!ACapstoneProjectGameModeBase::activeFactions.Contains(hexOwner))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("No faction found")));
		return;
	}

	if (harvesting)
	{
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Food].maxResources += value;
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Production].maxResources += value;
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[StratResources::Energy].maxResources += value;
	}
}

void ABaseHex::ToggleVisibility(Factions faction)
{
	if (!factionVisibility.Contains(faction))
	{
		factionVisibility.Add(faction, Visibility{ Undiscovered, false, false });
	}

	factionVisibility[faction].inSight = true;
}

void ABaseHex::SetVisibility()
{
	for (auto faction : factionVisibility)
	{
		VisibilityStatus originalVisibility = factionVisibility[faction.Key].status;

		if (faction.Value.inSight)
		{
			factionVisibility[faction.Key].status = Visible;
			factionVisibility[faction.Key].discoveredByFaction = true;

			if (factionVisibility[faction.Key].status != originalVisibility && !troopsInHex.IsEmpty())
			{
				for (int i = 0; i < troopsInHex.Num(); ++i)
				{
					if (troopsInHex[i]->unitStats->faction != Factions::Human)
						Cast<ATroop>(troopsInHex[i])->mesh->SetVisibility(true);
				}
			}
		}
		else
		{
			factionVisibility[faction.Key].status = (factionVisibility[faction.Key].discoveredByFaction) ? Discovered : Undiscovered;

			if (factionVisibility[faction.Key].status != originalVisibility && !troopsInHex.IsEmpty())
			{
				for (int i = 0; i < troopsInHex.Num(); ++i)
				{
					if (troopsInHex[i]->unitStats->faction != Factions::Human)
						Cast<ATroop>(troopsInHex[i])->mesh->SetVisibility(false);
				}
			}
		}
		factionVisibility[faction.Key].inSight = false;
		if (faction.Key == Factions::Human)
		{
			visibilityToHumans = faction.Value.status;
		}
	}
}

