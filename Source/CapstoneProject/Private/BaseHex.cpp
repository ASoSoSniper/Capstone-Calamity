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
		FText::FromString(TEXT("Flat terrain with no unique benefits.")), 2, 2, 1, 0, 0, 1.f, 1.f, 0,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Plain.Landscape_Icon_Plain'")) });

	hexInfo.Add(TerrainType::Forest, FHexInfo{ FText::FromString(TEXT("Fungal Forest")), 
		FText::FromString(TEXT("Full of tall mushrooms and food.")), 3, 1, 2, 0, 1, 1.15f, 1.1f, 0, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Forest.Landscape_Icon_Forest'")) });

	hexInfo.Add(TerrainType::Jungle, FHexInfo{ FText::FromString(TEXT("Oozing Jungle")), 
		FText::FromString(TEXT("Dense, humid, and sticky fungal growths.")), 2, 1, 2, 0, 1, 1.3f, 1.2f, -1, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Dense_Forest.Landscape_Icon_Dense_Forest'")) });

	hexInfo.Add(TerrainType::Hills, FHexInfo{ FText::FromString(TEXT("Capped Hills")), 
		FText::FromString(TEXT("Compressed mushroom caps create hills.")), 1, 3, 1, 0, 1, 1.2f, 1.1f, 1, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Hills_TEMP.Landscape_Icon_Hills_TEMP'")) });

	hexInfo.Add(TerrainType::Mountains, FHexInfo{ FText::FromString(TEXT("Stemstack Mountains")), 
		FText::FromString(TEXT("High intensity of mushroom stems in mountain formations make traversal and construction impossible.")), 0, 0, 0, 0, 0, 0, 0, -3,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Mountain.Landscape_Icon_Mountain'")) });

	hexInfo.Add(TerrainType::SporeField, FHexInfo{ FText::FromString(TEXT("Toxic Spore Field")), 
		FText::FromString(TEXT("Dangerous, toxic spores provide energy.")), 1, 1, 3, 0, 0, 1.2f, 1.0f, 0, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Toxic.Landscape_Icon_Toxic'")) });

	hexInfo.Add(TerrainType::Ship, FHexInfo{ FText::FromString(TEXT("Capitol Hub")), 
		FText::FromString(TEXT("The crash site of the ship, now the base of operations.")), 2, 3, 4, 0, 2, 1.0f, 1.0f, 0,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Capital_Hub/Hills_Capital_Hub.Hills_Capital_Hub'")) });

	hexInfo.Add(TerrainType::AlienCity, FHexInfo{ FText::FromString(TEXT("Normal Klequeen City")), 
		FText::FromString(TEXT("Alien city, some stupid piece of shit you shouldn't use.")), 3, 2, 1, 0, 2, 1.f, 1.f, 0,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Alien_city/Siatus_City.Siatus_City'")) });

	hexInfo.Add(TerrainType::TheRock, FHexInfo{ FText::FromString(TEXT("The Rock City")), 
		FText::FromString(TEXT("The Rock, contains DST fuel.")), 3, 3, 3, 0, 3, 1.f, 1.f, 1, 
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
	visibility->visibilityRadius = 50.f;
	visibility->detectionDistanceInRadius = 40.f;

	audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	audioComponent->SetupAttachment(RootComponent);

	//Initialize worker types
	workersInHex.Add(WorkerType::Human, 0);
	workersInHex.Add(WorkerType::Robot, 0);
	workersInHex.Add(WorkerType::Alien, 0);

	//Initialize resource yields
	resourceBonuses.Add(StratResources::Wealth, 1 );
	resourceBonuses.Add(StratResources::Energy, 1);
	resourceBonuses.Add(StratResources::Production, 1);
	resourceBonuses.Add(StratResources::Food, 1);
}

// Called when the game starts or when spawned
void ABaseHex::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseHex::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((building && !IsBuildableTerrain()) || (!troopsInHex.IsEmpty() && !IsTraversableTerrain()))
	{
		SetHexTerrain(TerrainType(FMath::RandRange(1, 4)));
	}

	if (hexOwner != Factions::None) ActiveHarvesting();

	if (attackingTroop)
	{ 
		if (!troopsInHex.Contains(attackingTroop)) 
			attackingTroop = nullptr;

		BeginBattle();
	}

	SetToTargetVolume(DeltaTime);
}

Factions ABaseHex::GetHexOwner()
{
	return hexOwner;
}

void ABaseHex::SetHexOwner(Factions faction)
{
	if (hexOwner != Factions::None)
	{
		if (ACapstoneProjectGameModeBase::activeFactions[hexOwner]->ownedHexes.Contains(this))
			ACapstoneProjectGameModeBase::activeFactions[hexOwner]->ownedHexes.Remove(this);
	}

	hexOwner = faction;

	if (faction == Factions::None)
	{
		visibility->hexBaseMaterials.visibleTexture = ACapstoneProjectGameModeBase::factionColors[Factions::None];
		return;
	}

	visibility->hexBaseMaterials.visibleTexture = ACapstoneProjectGameModeBase::activeFactions[faction]->factionColor;

	if (!ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes.Contains(this))
	{
		ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes.Add(this);
	}
}

int ABaseHex::GetMovementMulti()
{
	return moveMultiplier;
}

int ABaseHex::GetAttritionMulti()
{
	return attritionMultiplier;
}

int ABaseHex::GetDefenderBonus()
{
	return defenderBonus;
}

int ABaseHex::GetVision()
{
	return vision;
}

TArray<AActor*> ABaseHex::GetObjectsInHex()
{
	TArray<AActor*> actors;

	if (building) 
	{
		actors.Add(building);
	}

	if (battle)
	{
		actors.Add(battle);
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
		if (UnitActions::IsHostileTarget(refTroop, building) && building->unitStats->currentHP > 0)
		{
			BeginBattle(refTroop);
			return;
		}
	}
	for (int i = 0; i < troopsInHex.Num(); ++i)
	{
		if (troopsInHex[i] != refTroop)
		{
			if (UnitActions::IsHostileTarget(refTroop, troopsInHex[i]))
			{
				BeginBattle(refTroop);
				return;
			}
		}
	}
}

void ABaseHex::CheckForHostility(ABuilding* refBuilding)
{
	if (refBuilding->IsOccupied()) return;

	for (int i = 0; i < troopsInHex.Num(); ++i)
	{
		if (UnitActions::IsHostileTarget(troopsInHex[i], refBuilding))
		{
			BeginBattle(troopsInHex[i]);
			return;
		}
	}
}

void ABaseHex::AddTroopToHex(AMovementAI* troop)
{
	troop->hexNav->SetCurrentHex(this);
	troopsInHex.Add(troop);

	CheckForHostility(troop);
}

void ABaseHex::AddBuildingToHex(ABuilding* newBuilding)
{
	newBuilding->hexNav->SetCurrentHex(this);
	building = newBuilding;

	if (hexOwner == Factions::None) SetHexOwner(newBuilding->unitStats->faction);

	CheckForHostility(newBuilding);
}

void ABaseHex::RemoveTroopFromHex(AMovementAI* troop)
{
	if (troopsInHex.Contains(troop))
	{
		troopsInHex.Remove(troop);
	}
}

void ABaseHex::BeginBattle(AMovementAI* attacker)
{
	if (attacker && !attackingTroop && !ActiveBattleOnHex())
	{
		attackingTroop = attacker;
	}

	if (battle) return;

	if (attackingTroop) AGlobalSpawner::spawnerObject->SpawnBattle(this);
}

Factions ABaseHex::GetAttackerFaction()
{
	Factions faction = attackingTroop->unitStats->faction;
	attackingTroop = nullptr;

	return faction;
}

bool ABaseHex::ActiveBattleOnHex()
{
	if (!battle) return false;

	return battle->IsAttacking();
}

int ABaseHex::GetMaxWorkers()
{
	return maxWorkers;
}

void ABaseHex::SetMaxWorkers(int newMax)
{
	maxWorkers = newMax > 0 ? newMax : 0;
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
		float originalOutput = outputPercent * (float)resourceBonuses[resource.Key];
		float newOutput = newOutputPercent * (float)resourceBonuses[resource.Key];

		resource.Value.resourcePerTick -= FMath::RoundToInt(originalOutput);
		resource.Value.resourcePerTick += FMath::RoundToInt(newOutput);
	}

	if (debug) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, FString::Printf(TEXT("Output Percent: %f"), newOutputPercent));

	outputPercent = newOutputPercent;
}

ABuilding* ABaseHex::GetBuilding()
{
	return building;
}

void ABaseHex::SetBuilding(ABuilding* setBuilding)
{
	if (attachmentCanBeVisible)
	{
		hexMeshAttachment->SetVisibility(setBuilding == nullptr);
	}

	if (setBuilding) SetHexOwner(setBuilding->unitStats->faction);

	if (building && !setBuilding)
	{
		SetMaxWorkers(maxWorkersDefault);
		int totalWorkers = 0;
		for (auto& worker : workersInHex)
		{
			totalWorkers += worker.Value;
		}
		int workersToRemove = totalWorkers > maxWorkersDefault ? totalWorkers - maxWorkersDefault : 0;

		int removedWorkers = 0;
		while (removedWorkers < workersToRemove)
		{
			for (auto& worker : workersInHex)
			{
				if (removedWorkers == workersToRemove) break;

				int remove = UnitActions::RemoveWorkers(building->unitStats->faction, worker.Key, 1, this);

				if (remove > 0)
				{
					removedWorkers++;
					workersInHex[worker.Key]--;
				}
			}
		}
	}

	building = setBuilding;
}

int ABaseHex::GetNumberOfWorkers()
{
	int totalWorkers = 0;

	for (auto& worker : workersInHex)
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

float ABaseHex::GetTargetVolume()
{
	return targetVolume;
}

void ABaseHex::SetTargetVolume(float volume)
{
	targetVolume = volume > 0 ? volume : 0;
}

void ABaseHex::SetInSoundBoxRadius(bool inRadius)
{
	inSoundboxRadius = inRadius;
}

FHexDisplay ABaseHex::GetDisplayInfo()
{
	FHexDisplay display;

	display.name = hexInfo[hexTerrain].name;
	display.description = hexInfo[hexTerrain].description;

	display.food = FText::AsNumber(resourceBonuses[StratResources::Food]);
	display.production = FText::AsNumber(resourceBonuses[StratResources::Production]);
	display.energy = FText::AsNumber(resourceBonuses[StratResources::Energy]);
	display.wealth = FText::AsNumber(resourceBonuses[StratResources::Wealth]);

	display.defenderBonus = FText::AsNumber(hexInfo[hexTerrain].defenderBonus);
	display.moveMultiplier = FText::AsNumber(hexInfo[hexTerrain].moveMultiplier);
	display.attritionBonus = FText::AsNumber(hexInfo[hexTerrain].attritionMultiplier);
	display.visionModifier = FText::AsNumber(hexInfo[hexTerrain].visionModifier);

	display.icon = hexInfo[hexTerrain].icon;

	int workers = 0;
	for (auto& worker : workersInHex)
	{
		workers += worker.Value;
	}
	TArray<FStringFormatArg> Args;
	Args.Add(workers);
	Args.Add(maxWorkers);
	display.workerCount = FText::FromString(FString::Format(TEXT("{0}/{1}"), Args));

	return display;
}

void ABaseHex::UpdateResourceYield(StratResources resource, int value, Factions faction)
{
	Factions selectedFaction = faction != Factions::None ? faction : hexOwner;

	resourceBonuses[resource] += value;

	if (!ACapstoneProjectGameModeBase::activeFactions.Contains(selectedFaction))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("No faction found")));
		return;
	}

	if (harvesting)
		ACapstoneProjectGameModeBase::activeFactions[selectedFaction]->resourceInventory[resource].resourcePerTick += value;
}

void ABaseHex::SetToTargetVolume(float& DeltaTime)
{
	int increase = audioComponent->VolumeMultiplier < targetVolume ? 1 : -1;

	float step = audioComponent->VolumeMultiplier + (DeltaTime * increase * volumeSpeed);
	step = increase == 1 ? FMath::Clamp(step, audioComponent->VolumeMultiplier, targetVolume) : FMath::Clamp(step, targetVolume, audioComponent->VolumeMultiplier);

	audioComponent->VolumeMultiplier = step;
	audioComponent->AdjustVolume(0.f, audioComponent->VolumeMultiplier);

	if (audioComponent->VolumeMultiplier <= 0.f && !inSoundboxRadius) audioComponent->Stop();
}

TerrainType ABaseHex::GetHexTerrain()
{
	return hexTerrain;
}

void ABaseHex::SetHexTerrain(int maxSeedSize, int randToMaintain)
{
	//Declare variable to use as return value
	TerrainType terrainToSet = TerrainType::None;

	//Get surrounding hexes
	TSet<ABaseHex*> surroundingHexes = GetSurroundingHexes();

	bool randTerrain = false;
	int highestSeed = 0;

	//Compare seed values for highest value, set return variable to hex with that value
	for (ABaseHex* hex : surroundingHexes)
	{
		if (hex->seedIndex > highestSeed)
		{
			highestSeed = hex->seedIndex;
			terrainToSet = hex->GetHexTerrain();
		}
	}

	//If the highest seed found is 0, either no seed was previously set, or the end of an existing seed was reached
	//Regardless, this is a prompt to set this hex to a new random terrain
	if (highestSeed <= 0)
	{
		randTerrain = true;
	}
	else
	{
		//If a seed was found, get a random number from 1-10 and add the found seed
		//This makes growth of the current seed stronger in its beginnings
		int rand = FMath::RandRange(1, 10) + seedIndex;

		//If the random number is greater than the minimum number to maintain, use the highest seed index terrain
		//Set this hex's seed index to the highest found minus 1 to reduce chances of continued growth in the next hex
		if (rand > randToMaintain)
		{
			seedIndex = highestSeed - 1;
		}
		else
		{
			//Otherwise, prompt the beginning of a new random terrain seed
			randTerrain = true;
		}
	}

	//If randomization was prompted, pick a random terrain type and set the seed index to the maximum
	if (randTerrain)
	{
		terrainToSet = TerrainType(FMath::RandRange(1, 6));
		seedIndex = maxSeedSize;
	}

	//Set the hex to the resulting terrain value
	SetHexTerrain(terrainToSet);
}

void ABaseHex::SetHexTerrain(TerrainType terrain)
{
	hexTerrain = terrain;

	FHexInfo info = hexInfo[hexTerrain];
	resourceBonuses[StratResources::Food] = info.food;
	resourceBonuses[StratResources::Production] = info.production;
	resourceBonuses[StratResources::Energy] = info.energy;
	resourceBonuses[StratResources::Wealth] = info.wealth;

	moveMultiplier = info.moveMultiplier;
	attritionMultiplier = info.attritionMultiplier;
	defenderBonus = info.defenderBonus;
	vision = info.visionModifier;

	if (hexTerrain == TerrainType::Ship)
	{
		visibility->enableScan = true;
		SetHexOwner(Factions::Human);
	}

	SetHexModel();
}

void ABaseHex::SetHexModel()
{
	if (!visibility->factionVisibility.Contains(Factions::Human)) return;

	if (!visibility->factionVisibility[Factions::Human].discoveredByFaction)
	{
		AGlobalSpawner::spawnerObject->CreateHexModel(TerrainType::None, this);
	}
	else
	{
		AGlobalSpawner::spawnerObject->CreateHexModel(hexTerrain, this);
	}
}

void ABaseHex::SetAttachmentCanBeVisible(bool canBeVisible)
{
	attachmentCanBeVisible = canBeVisible;
}

bool ABaseHex::IsStaticBuildingTerrain()
{
	return hexTerrain == TerrainType::Ship ||
		hexTerrain == TerrainType::TheRock ||
		hexTerrain == TerrainType::AlienCity;
}

bool ABaseHex::IsTraversableTerrain()
{
	return hexTerrain != TerrainType::Border && 
		hexTerrain != TerrainType::Mountains;
}

bool ABaseHex::IsBuildableTerrain()
{
	return hexTerrain != TerrainType::Border && 
		hexTerrain != TerrainType::Mountains && 
		hexTerrain != TerrainType::Jungle;
}

bool ABaseHex::IsPlayerHex()
{
	return hexOwner == Factions::Human;
}

bool ABaseHex::CanPutWorkersOnHex()
{
	return IsTraversableTerrain() && IsPlayerHex();
}

FCurrentResourceYields ABaseHex::GetCurrentResourceYields()
{
	FCurrentResourceYields yields;

	//float outputPercent = GetOutputPercent();

	yields.foodYield = FMath::RoundToInt(outputPercent * (float)resourceBonuses[StratResources::Food]);
	yields.energyYield = FMath::RoundToInt(outputPercent * (float)resourceBonuses[StratResources::Energy]);
	yields.productionYield = FMath::RoundToInt(outputPercent * (float)resourceBonuses[StratResources::Production]);
	yields.wealthYield = FMath::RoundToInt(outputPercent * (float)resourceBonuses[StratResources::Wealth]);

	return yields;
}

TSet<ABaseHex*> ABaseHex::GetSurroundingHexes()
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	TArray<AActor*> objectsInHex = GetObjectsInHex();
	if (objectsInHex.Num() > 0) queryParams.AddIgnoredActors(objectsInHex);

	TSet<ABaseHex*> hexesFound;

	//For each of the 6 directions:
	for (int i = 0; i < 6; i++)
	{
		//Establish trace direction
		FVector traceStart = GetActorLocation() + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * 10;
		FVector traceEnd = traceStart + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * 30;
		FHitResult hit;

		//Trace
		bool found = GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams, FCollisionResponseParams::DefaultResponseParam);

		//Check for hex actor class
		ABaseHex* foundHex = found ? Cast<ABaseHex>(hit.GetActor()) : nullptr;

		//Add found hex to set
		if (foundHex)
			hexesFound.Add(foundHex);
	}

	return hexesFound;
}
