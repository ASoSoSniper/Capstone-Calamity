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

	hexInfo.Add(TerrainType::Plains, FHexInfo{ FText::FromString(TEXT("Moldy Plains")), 
		FText::FromString(TEXT("Flat terrain with no unique benefits.")), 3, 2, 1, 0, 1.f, 1.f, 0,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Plain.Landscape_Icon_Plain'")) });

	hexInfo.Add(TerrainType::Forest, FHexInfo{ FText::FromString(TEXT("Fungal Forest")), 
		FText::FromString(TEXT("Full of tall mushrooms and food.")), 4, 3, 1, 1, 0.85f, 1.1f, 0, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Forest.Landscape_Icon_Forest'")) });

	hexInfo.Add(TerrainType::Jungle, FHexInfo{ FText::FromString(TEXT("Oozing Jungle")), 
		FText::FromString(TEXT("Dense, humid, and sticky fungal growths.")), 3, 3, 1, 1, 0.7f, 1.2f, -1, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Dense_Forest.Landscape_Icon_Dense_Forest'")) });

	hexInfo.Add(TerrainType::Hills, FHexInfo{ FText::FromString(TEXT("Capped Hills")), 
		FText::FromString(TEXT("Compressed mushroom caps create hills.")), 1, 4, 2, 1, 0.8f, 1.1f, 1, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Hills_TEMP.Landscape_Icon_Hills_TEMP'")) });

	hexInfo.Add(TerrainType::Mountains, FHexInfo{ FText::FromString(TEXT("Stemstack Mountains")), 
		FText::FromString(TEXT("High intensity of mushroom stems in mountain formations make traversal and construction impossible.")), 0, 0, 0, 0, 0, 0, 0,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Mountain.Landscape_Icon_Mountain'")) });

	hexInfo.Add(TerrainType::SporeField, FHexInfo{ FText::FromString(TEXT("Toxic Spore Field")), 
		FText::FromString(TEXT("Dangerous, toxic spores provide energy.")), 1, 2, 5, 0, 0.8f, 1.0f, -1, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Toxic.Landscape_Icon_Toxic'")) });

	hexInfo.Add(TerrainType::Ship, FHexInfo{ FText::FromString(TEXT("Capitol Hub")), 
		FText::FromString(TEXT("The crash site of the ship, now the base of operations.")), 2, 3, 4, 3, 1.0f, 1.0f, 0,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Capital_Hub/Hills_Capital_Hub.Hills_Capital_Hub'")) });

	hexInfo.Add(TerrainType::AlienCity, FHexInfo{ FText::FromString(TEXT("Normal Klequeen City")), 
		FText::FromString(TEXT("Alien city, some stupid piece of shit you shouldn't use.")), 3, 2, 1, 2, 1.f, 1.f, 0,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Alien_city/Siatus_City.Siatus_City'")) });

	hexInfo.Add(TerrainType::TheRock, FHexInfo{ FText::FromString(TEXT("The Rock City")), 
		FText::FromString(TEXT("The Rock, contains DST fuel.")), 2, 3, 4, 3, 1.f, 1.f, 1, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Alien_city/Rock_City_Octagon.Rock_City_Octagon'")) });

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

	hexBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hex Base Mesh"));
	hexBase->SetupAttachment(RootComponent);
	UStaticMesh* baseAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/CyTheHexGuy.CyTheHexGuy'"));
	if (baseAsset)
	{
		hexBase->SetStaticMesh(baseAsset);
	}
	hexBase->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.5f));
	hexBase->SetRelativeLocation(FVector(0.f, 0.f, -4.f));

	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));
	interactable->CreateExtraCollision(hexMeshAttachment);

	visibility = CreateDefaultSubobject<UMeshVisibility>(TEXT("Mesh Visibility"));
	visibility->enableScan = false;
	visibility->hexBaseMesh = hexBase;

	//Initialize worker types
	workersInHex.Add(WorkerType::Human, 0);
	workersInHex.Add(WorkerType::Robot, 0);
	workersInHex.Add(WorkerType::Alien, 0);

	//Initialize resource yields
	resourceBonuses.Add(StratResources::Wealth, ResourceStats{ wealthYieldBonus });
	resourceBonuses.Add(StratResources::Energy, ResourceStats{ energyYieldBonus});
	resourceBonuses.Add(StratResources::Production, ResourceStats{ productionYieldBonus});
	resourceBonuses.Add(StratResources::Food, ResourceStats{ foodYieldBonus});
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
	if ((building && (hexTerrain == TerrainType::Mountains || hexTerrain == TerrainType::Jungle)) || (!troopsInHex.IsEmpty() && hexTerrain == TerrainType::Mountains))
	{
		terrainChange = TerrainType(FMath::RandRange(1, 4));
	}

	if (spawner && terrainChange != hexTerrain)
	{
		RequestTerrainChange();
	}

	if (hexOwner != Factions::None) ActiveHarvesting();
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
	if (building)
	{
		if (UnitActions::IsHostileTarget(refTroop, building) && !building->disabled)
		{
			BeginBattle();
			return;
		}
	}
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


bool ABaseHex::ActiveHarvesting()
{
	bool alreadyHarvesting = harvesting;
	bool workersExist = false;

	if (GetNumberOfWorkers() >= 1) workersExist = true;

	harvesting = workersExist;

	if (harvesting != alreadyHarvesting)
	{
		if (building) building->visibility->enableScan = harvesting;
	}

	if (GetOutputPercent() != outputPercent)
	{
		ToggleResourceYield();
	}

	return workersExist;
}

void ABaseHex::ToggleResourceYield()
{
	if (hexOwner == Factions::None) return;

	float newOutputPercent = GetOutputPercent();

	for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory)
	{
		float originalOutput = outputPercent * (float)resourceBonuses[resource.Key].yieldBonus;
		float newOutput = newOutputPercent * (float)resourceBonuses[resource.Key].yieldBonus;

		resource.Value.resourcePerTick -= originalOutput;
		resource.Value.resourcePerTick += newOutput;
	}

	if (debug) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("Output Percent: %f"), newOutputPercent));

	outputPercent = newOutputPercent;
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

	moveMultiplier = info.moveMultiplier;
	attritionMultiplier = info.attritionMultiplier;
	defenderBonus = info.defenderBonus;
	vision = info.visionModifier;
}

int ABaseHex::GetNumberOfWorkers()
{
	int totalWorkers = 0;

	for (auto worker : workersInHex)
	{
		totalWorkers += worker.Value;
	}

	return totalWorkers;
}

float ABaseHex::GetOutputPercent()
{
	return (float)GetNumberOfWorkers() / (float)maxWorkers;
}

ABaseHex* ABaseHex::FindFreeAdjacentHex(Factions faction, TArray<ABaseHex*> ignoredHexes)
{
	//if (this == nullptr) return nullptr;
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
		//GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Orange, TEXT("WEEEEEEEEEEEEEE"));
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

	display.defenderBonus = FText::AsNumber(hexInfo[hexTerrain].defenderBonus);
	display.moveMultiplier = FText::AsNumber(hexInfo[hexTerrain].moveMultiplier);
	display.attritionBonus = FText::AsNumber(hexInfo[hexTerrain].attritionMultiplier);
	display.visionModifier = FText::AsNumber(hexInfo[hexTerrain].visionModifier);

	display.icon = hexInfo[hexTerrain].icon;

	int workers = 0;
	for (auto worker : workersInHex)
	{
		workers += worker.Value;
	}
	TArray<FStringFormatArg> Args;
	Args.Add(workers);
	Args.Add(maxWorkers);
	display.workerCount = FText::FromString(FString::Format(TEXT("{0}/{1}"), Args));

	return display;
}

void ABaseHex::UpdateResourceYield(StratResources resource, int value)
{
	resourceBonuses[resource].yieldBonus += value;

	if (!ACapstoneProjectGameModeBase::activeFactions.Contains(hexOwner))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("No faction found")));
		return;
	}

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[hexOwner]->resourceInventory[resource].resourcePerTick += value;
}

void ABaseHex::SetFaction(Factions faction)
{
	UnitActions::AssignFaction(faction, this);
}