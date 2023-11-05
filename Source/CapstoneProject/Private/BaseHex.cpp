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
	resourceBonuses.Add(StratResources::Energy, energyYieldBonus);
	resourceBonuses.Add(StratResources::Production, productionYieldBonus);
	resourceBonuses.Add(StratResources::Food, foodYieldBonus);

	//Initialize worker types
	workersInHex.Add(WorkerType::Human, 0);
	workersInHex.Add(WorkerType::Robot, 0);
	workersInHex.Add(WorkerType::Alien, 0);
}

// Called when the game starts or when spawned
void ABaseHex::BeginPlay()
{
	Super::BeginPlay();
	
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

	if (!harvesting) return;

	//Harvest(DeltaTime);
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
		UnitActions::HarvestResources(hexOwner, resourceBonuses[StratResources::Food], StratResources::Food);
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
	resourceBonuses[StratResources::Food] += value;

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Food] += value;
	
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Food = %d"), ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Food]));
}

void ABaseHex::UpdateProductionYield(int value)
{
	resourceBonuses[StratResources::Production] += value;

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Production] += value;
	
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Production = %d"), ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Production]));
}

void ABaseHex::UpdateEnergyYield(int value)
{
	resourceBonuses[StratResources::Energy] += value;

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Energy] += value;

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Energy = %d"), ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Energy]));
}

void ABaseHex::ToggleResourceYield()
{
	if (hexOwner == Factions::None) return;

	int axis = harvesting ? 1 : -1;

	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Food] += axis * resourceBonuses[StratResources::Food];
	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Production] += axis * resourceBonuses[StratResources::Energy];
	ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourcePerTick[StratResources::Energy] += axis * resourceBonuses[StratResources::Energy];
}

