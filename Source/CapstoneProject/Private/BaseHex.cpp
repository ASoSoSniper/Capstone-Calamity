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

	hexInfo.Add(TerrainType::Plains, FHexInfo{ FText::FromString(TEXT("Moldy Plains")), FText::FromString(TEXT("Flat terrain with no unique benefits.")), 3, 2, 1 });
	hexInfo.Add(TerrainType::Forest, FHexInfo{ FText::FromString(TEXT("Fungal Forest")),FText::FromString(TEXT("Full of tall mushrooms and food.")), 4, 3, 1 });
	hexInfo.Add(TerrainType::Jungle, FHexInfo{ FText::FromString(TEXT("Oozing Jungle")),FText::FromString(TEXT("Dense, humid, and sticky fungal growths.")), 3, 3, 1 });
	hexInfo.Add(TerrainType::Hills, FHexInfo{ FText::FromString(TEXT("Capped Hills")),FText::FromString(TEXT("Compressed mushroom caps create a hills.")), 1, 4, 2 });
	hexInfo.Add(TerrainType::Mountains, FHexInfo{ FText::FromString(TEXT("Stemstack Mountains")),FText::FromString(TEXT("High intensity of mushroom stems in mountain formations make traversal and construction impossible.")), 0, 0, 0 });
	hexInfo.Add(TerrainType::SporeField, FHexInfo{ FText::FromString(TEXT("Toxic Spore Field")),FText::FromString(TEXT("Dangerous, toxic spores provide energy.")), 1, 2, 5 });
	hexInfo.Add(TerrainType::Ship, FHexInfo{ FText::FromString(TEXT("Capitol Hub")),FText::FromString(TEXT("The crash site of the ship, now the base of operations.")), 2, 3, 4 });
	hexInfo.Add(TerrainType::AlienCity, FHexInfo{ FText::FromString(TEXT("Normal Klequeen City")),FText::FromString(TEXT("Alien city, some stupid piece of shit you shouldn't use.")), 3, 2, 1 });
	hexInfo.Add(TerrainType::TheRock, FHexInfo{ FText::FromString(TEXT("The Rock City")),FText::FromString(TEXT("The Rock, contains DST fuel.")), 2, 3, 4 });

	hexMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hex Mesh"));
	RootComponent = hexMesh;
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TilePlains.TilePlains'"));
	if (meshAsset)
	{
		hexMesh->SetStaticMesh(meshAsset);
	}	

	troopAnchor = CreateDefaultSubobject<USceneComponent>("Troop Anchor");
	troopAnchor->SetupAttachment(RootComponent);

	buildingAnchor = CreateDefaultSubobject<USceneComponent>("Building Anchor");
	buildingAnchor->SetupAttachment(RootComponent);

	hexMeshAttachment = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Attachment"));
	hexMeshAttachment->SetupAttachment(RootComponent);

	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));
	interactable->CreateExtraCollision(hexMeshAttachment);

	visibility = CreateDefaultSubobject<UMeshVisibility>(TEXT("Mesh Visibility"));

	//Initialize worker types
	workersInHex.Add(WorkerType::Human, 0);
	workersInHex.Add(WorkerType::Robot, 0);
	workersInHex.Add(WorkerType::Alien, 0);

	//Initialize resource yields
	resourceBonuses.Add(StratResources::Energy, ResourceStats{ energyYieldBonus, 0 });
	resourceBonuses.Add(StratResources::Production, ResourceStats{ productionYieldBonus, 0 });
	resourceBonuses.Add(StratResources::Food, ResourceStats{ foodYieldBonus, 0 });
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
	
	if (spawner) RequestTerrainChange();

	currentHarvestTime = maxHarvestTime;
}

// Called every frame
void ABaseHex::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}
	if (spawner && terrainChange != hexTerrain)
	{
		RequestTerrainChange();
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

void ABaseHex::RequestTerrainChange(bool modelOnly)
{
	if (visibility->factionVisibility.Num() < ACapstoneProjectGameModeBase::activeFactions.Num()) return;

	if (terrainChange != TerrainType::None) hexTerrain = terrainChange;
	else (terrainChange = hexTerrain);

	if (!visibility->factionVisibility[Factions::Human].discoveredByFaction)
	{
		spawner->CreateHexModel(TerrainType::None, this);
	}
	else
	{
		spawner->CreateHexModel(hexTerrain, this);
	}

	if (modelOnly) return;

	FHexInfo info = hexInfo[hexTerrain];
	resourceBonuses[StratResources::Food].yieldBonus = info.food;
	resourceBonuses[StratResources::Production].yieldBonus = info.production;
	resourceBonuses[StratResources::Energy].yieldBonus = info.energy;
}

ABaseHex* ABaseHex::FindFreeAdjacentHex(Factions faction, TArray<ABaseHex*> ignoredHexes)
{
	//Return this hex if no troops in this faction are positioned on it
	bool factionInThis = false;
	for (int j = 0; j < troopsInHex.Num(); j++)
	{
		if (troopsInHex[j]->unitStats->faction == faction) factionInThis = true;
	}
	if (!factionInThis)
	{
		for (int i = 0; i < ignoredHexes.Num(); i++)
		{
			if (ignoredHexes[i] == this) factionInThis = true;
		}
		if (!factionInThis) return this;
	}

	//Otherwise, check surrounding hexes for a free spot and return the first available
	TArray<AActor*> adjacentHexes;
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);

	bool found = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), 50.f, TArray<TEnumAsByte<EObjectTypeQuery>>(), ABaseHex::StaticClass(), actorsToIgnore, adjacentHexes);
	if (!found) return this;

	for (int i = 0; i < adjacentHexes.Num(); i++)
	{
		ABaseHex* hex = Cast<ABaseHex>(adjacentHexes[i]);

		for (int k = 0; k < ignoredHexes.Num(); k++)
		{
			if (ignoredHexes[k] == adjacentHexes[i]) continue;
		}

		if (hex->hexTerrain == TerrainType::Mountains || hex->hexTerrain == TerrainType::Border) continue;

		bool factionInHex = false;
		for (int j = 0; j < hex->troopsInHex.Num(); j++)
		{
			if (hex->troopsInHex[j]->unitStats->faction == faction) factionInHex = true;
		}
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Orange, TEXT("WEEEEEEEEEEEEEE"));
		if (!factionInHex) return hex;
	}
	return this;
}

FHexDisplay ABaseHex::GetDisplayInfo()
{
	FHexDisplay display;

	display.name = hexInfo[hexTerrain].name;
	display.description = hexInfo[hexTerrain].description;

	display.food = FText::AsNumber(resourceBonuses[StratResources::Food].yieldBonus);
	display.production = FText::AsNumber(resourceBonuses[StratResources::Production].yieldBonus);
	display.energy = FText::AsNumber(resourceBonuses[StratResources::Energy].yieldBonus);

	int workers = 0;
	for (auto worker : workersInHex)
	{
		workers += worker.Value;
	}
	TArray<FStringFormatArg> Args;
	Args.Add(workers);
	Args.Add(10);
	display.workerCount = FText::FromString(FString::Format(TEXT("{0}/{1}"), Args));

	return display;
}

