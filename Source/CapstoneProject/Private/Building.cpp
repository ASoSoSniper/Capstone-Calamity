// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "CapstoneProjectGameModeBase.h"
#include "GlobalSpawner.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));	
	RootComponent = mesh;
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingInProgress.BuildingInProgress'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
	mesh->SetCollisionProfileName("BlockAllDynamic");

	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interaction"));
	hexNav = CreateDefaultSubobject<UHexNav>(TEXT("Hex Nav"));
	visibility = CreateDefaultSubobject<UMeshVisibility>(TEXT("Mesh Visibility"));
	visibility->enableScan = false;

	buildingType = SpawnableBuildings::None;
	preAssignedFaction = Factions::None;

	resourceYields.Add(StratResources::Energy, 0);
	resourceYields.Add(StratResources::Food, 0);
	resourceYields.Add(StratResources::Production, 0);
	resourceYields.Add(StratResources::Wealth, 0);

	cinematicComponent = CreateDefaultSubobject<UCinematicPosition>(TEXT("Cinematic Component"));
	cinematicComponent->cinematicVars = FCinematicObject{ this, FVector::Zero(), 1.5f, 1.5f};
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	if (preAssignedFaction != Factions::None)
		InitBuilding(preAssignedFaction);
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!setupComplete)
	{
		setupComplete = SetupBuilding(buildingType);
	}

	switch (buildState)
	{
	case Building:
		Constructing(DeltaTime);
		break;
	case Complete:
		UpdateUnrestLevel(DeltaTime);
		break;
	case Destroying:
		DestroyingBuilding(DeltaTime);
		break;
	}
}

FUnitData* ABuilding::GetUnitData() const
{
	return unitData;
}

void ABuilding::InitBuilding(const Factions& faction)
{
	using GameMode = ACapstoneProjectGameModeBase;
	if (!GameMode::activeFactions.Contains(faction)) return;

	unitData = new FUnitData(faction);
	if (GameMode::activeFactions.Contains(faction))
		GameMode::activeFactions[faction]->allBuildings.Add(this);

	visibility->SetupComponent(unitData);
	cinematicComponent->cinematicVars.position = GetActorLocation() + cinematicComponent->positionOffset;
}

bool ABuilding::SetupBuilding(SpawnableBuildings type)
{
	if (!AGlobalSpawner::spawnerObject)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No global spawner, could not set up building"));
		return false;
	}
	if (!AGlobalSpawner::spawnerObject->buildingStats.Contains(type))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No building stats found, could not set up building"));
		return false;
	}
	if (!unitData || unitData->GetFaction() == Factions::None)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No faction set, could not set up building"));
		return false;
	}
	
	//If the building's hex is not set, scan until a hex is found
	if (!hexNav->GetCurrentHex())
	{
		SphereCheck();
	}

	//If a hex is not found, cancel the process
	if (!hexNav->GetCurrentHex())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No hex found, could not set up building"));
		return false;
	}

	//Get the building data
	FBuildingStats stats = AGlobalSpawner::spawnerObject->buildingStats[type];

	unitData->SetBuildingValues(stats.HP, stats.vision, stats.siegeDamage, stats.energyUpkeepCost);

	resourceYields[StratResources::Energy] = stats.energyYield;
	resourceYields[StratResources::Food] = stats.foodYield;
	resourceYields[StratResources::Production] = stats.productionYield;
	resourceYields[StratResources::Wealth] = stats.wealthYield;

	FBuildingCost costs;
	ABaseHex* hex = hexNav->GetCurrentHex();

	if (AGlobalSpawner::spawnerObject->buildingCosts.Contains(type))
	{
		costs = AGlobalSpawner::spawnerObject->buildingCosts[type];

		if (hex->GetMaxWorkers() != costs.workerCost)
		{
			hex->SetMaxWorkers(costs.workerCost);
		}
	}

	int hexWorkers = 0;
	int availableWorkers = 0;
	TMap<WorkerType, int> factionWorkers = UnitActions::GetFactionWorkers(unitData->GetFaction());
	for (auto& worker : hex->workersInHex)
	{
		hexWorkers += worker.Value;
	}
	for (auto& worker : factionWorkers)
	{
		availableWorkers += worker.Value;
	}

	while (hexWorkers < hex->GetMaxWorkers() && availableWorkers > 0)
	{
		for (auto& worker : factionWorkers)
		{
			int addedWorker = UnitActions::AddWorkers(unitData->GetFaction(), worker.Key, 1, hex);

			hex->workersInHex[worker.Key] += addedWorker;
			hexWorkers += addedWorker;
			availableWorkers -= addedWorker;

			if (hexWorkers >= hex->GetMaxWorkers() || availableWorkers <= 0) break;
		}
	}

	resourceCapIncrease = stats.resourceCapIncrease;

	if (!builtAtStart)
	{
		buildTime = costs.timeToBuild;
		currBuildTime = costs.timeToBuild;
		SetBuildState();
	}
	else
	{
		buildState = Building;
	}

	return true;
}

void ABuilding::Constructing(float& DeltaTime)
{
	currBuildTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
	if (currBuildTime <= 0.f)
	{
		SetBuildState();
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Build complete!"));
	}
}

void ABuilding::SetBuildState()
{
	switch (buildState)
	{
	case None:
		buildState = Building;
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Begun building"));
		break;
	case Building:
		buildState = Complete;
		UpdateResources();
		BuildingAction();
		SetToFinishedModel();
		visibility->enableScan = true;
		if (unitData->GetFaction() == Factions::Human) AGlobalSpawner::spawnerObject->controller->PlayUISound(AGlobalSpawner::spawnerObject->controller->buildingCompleteSound);
		break;
	case Complete:
		
		break;
	case Uprading:
		buildState = Upgraded;
		break;
	}
}

void ABuilding::UpdateResources()
{
	ABaseHex* hex = hexNav->GetCurrentHex();
	if (!hex) return;

	for (auto& resource : resourceYields)
	{
		int value = resource.Value;
		if (occupyingFaction != Factions::None)
		{
			int ownerPortion = FMath::RoundToInt((float)value - (float)value * occupyResourcePercent);
			int siegePortion = FMath::RoundToInt((float)value * occupyResourcePercent);
			hex->UpdateResourceYield(resource.Key, ownerPortion, unitData->GetFaction());
			hex->UpdateResourceYield(resource.Key, siegePortion, occupyingFaction);
		}
		else
		{
			hex->UpdateResourceYield(resource.Key, value);
		}
	}

	UnitActions::UpdateResourceCapacity(unitData->GetFaction(), resourceCapIncrease);
}

void ABuilding::RevertResources()
{
	ABaseHex* hex = hexNav->GetCurrentHex();
	if (!hex) return;

	for (auto& resource : resourceYields)
	{
		hex->UpdateResourceYield(resource.Key, -resource.Value);
	}
}

void ABuilding::BuildingAction()
{
}

void ABuilding::SetToFinishedModel()
{
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/CysAwfulBuilding.CysAwfulBuilding'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
}

bool ABuilding::SphereCheck()
{
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	TArray<FHitResult> results;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetActorLocation(), GetActorLocation(), hexSearchDistance, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::None, results, true);

	if (bHit)
	{
		for (int i = 0; i < results.Num(); i++)
		{
			ABaseHex* hexActor = Cast<ABaseHex>(results[i].GetActor());
			if (hexActor && results[i].GetActor() != hexNav->GetCurrentHex())
			{
				if (FMath::Abs(GetActorLocation().X - hexActor->GetActorLocation().X) < hexSnapDistance && FMath::Abs(GetActorLocation().Y - hexActor->GetActorLocation().Y) < hexSnapDistance)
				{
					SetActorLocation(hexActor->buildingAnchor->GetComponentLocation());
					hexActor->AddBuildingToHex(this, GetHexLayersToOccupy());

					return true;
				}
			}
		}
	}

	return false;
}

void ABuilding::Action1()
{
}

void ABuilding::Action2()
{
}

void ABuilding::Action3()
{
}

void ABuilding::Action4()
{
}

void ABuilding::Action5()
{
}

void ABuilding::Action6()
{
}

void ABuilding::Action7()
{
}

void ABuilding::Action8()
{
}

void ABuilding::Action9()
{
}

void ABuilding::Action10()
{
}

void ABuilding::BeginDestroying()
{
	if (buildState == Destroying) return;

	if (buildingType == SpawnableBuildings::Capitol || 
		buildingType == SpawnableBuildings::AlienCity || 
		buildingType == SpawnableBuildings::RockCity) return;

	currDestructionTime = destructionTime;
	buildState = Destroying;
}

void ABuilding::DestroyingBuilding(float& DeltaTime)
{
	if (currDestructionTime > 0.f)
	{
		currDestructionTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		return;
	}

	Destroy();
}

void ABuilding::Destroyed()
{
	ABaseHex* hex = hexNav->GetCurrentHex();

	if (hex)
	{
		Factions faction = unitData->GetFaction();

		UnitActions::UpdateResourceCapacity(faction, -resourceCapIncrease);

		if (ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings.Contains(this))
		{
			ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings.Remove(this);
		}

		if (AGlobalSpawner::spawnerObject->buildingCosts.Contains(buildingType))
		{
			TMap<StratResources, int> addResources;
			addResources.Add(StratResources::Production, AGlobalSpawner::spawnerObject->buildingCosts[buildingType].productionCost * 0.25f);
			UnitActions::AddResources(faction, addResources);
		}

		hex->RemoveBuildingFromHex(GetHexLayersToOccupy());
	}

	if (smokeEffect)
	{
		smokeEffect->Destroy();
		smokeEffect = nullptr;
	}

	Super::Destroyed();
}

bool ABuilding::IsDisabled()
{
	if (occupied) return true;
	if (unitData->IsAlive()) return false;

	return true;
}

bool ABuilding::SetSiegeState(bool sieging, Factions occupier)
{
	if (occupied == sieging) return false;

	occupied = sieging;

	if (occupied)
	{
		smokeEffect = AGlobalSpawner::spawnerObject->SpawnSmoke(this);
		occupyingFaction = occupier;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("occupation changed"));
	}
	else
	{
		occupyingFaction = Factions::None;
		if (smokeEffect)
		{
			smokeEffect->Destroy();
			smokeEffect = nullptr;
		}
	}

	RevertResources();
	UpdateResources();

	return true;
}

bool ABuilding::IsOccupied()
{
	return occupied;
}

void ABuilding::SetBuildAtStart(bool active)
{
	if (setupComplete) return;

	builtAtStart = active;
}

bool ABuilding::TroopOccupation()
{
	return GetOccupyingTroops() >= troopOccupationMin;
}

int ABuilding::GetOccupyingTroops()
{
	ABaseHex* hex = hexNav->GetCurrentHex();

	int occupyingTroops = 0;

	for (int i = 0; i < hex->troopsInHex.Num(); i++)
	{
		FUnitData* unit = hex->troopsInHex[i]->GetUnitData();

		if (unit->GetFaction() == occupyingFaction)
		{
			if (unit->GetUnitType() == UnitTypes::Army)
			{
				occupyingTroops += unit->GetSavedUnitCount();
			}
			else
			{
				occupyingTroops++;
			}
		}
	}

	return occupyingTroops;
}

int ABuilding::GetOccupationMinCount()
{
	return troopOccupationMin;
}

void ABuilding::UpdateUnrestLevel(float& DeltaTime)
{
	if (!occupied)
	{
		unrestLevel = 0.f; 
		return;
	}
	if (TroopOccupation())
	{
		unrestLevel = 0.f;
		SetSiegeState(true);
		return;
	}
	unrestLevel += (DeltaTime / unrestTime) * ACapstoneProjectGameModeBase::timeScale;
	unrestLevel = FMath::Clamp(unrestLevel, 0.f, 1.f);
	if (unrestLevel < 1.f) return;

	SetSiegeState(false);
}

Factions ABuilding::GetOccupier()
{
	return occupyingFaction;
}

float ABuilding::GetBuildPercent()
{
	if (buildTime <= 0) return 0.f;

	return (buildTime - currBuildTime) / buildTime;
}

float ABuilding::GetUnrestPercent()
{
	return unrestLevel;
}

SpawnableBuildings ABuilding::GetBuildingType()
{
	return buildingType;
}

bool ABuilding::ConstructionComplete() const
{
	return buildState == BuildStates::Complete || buildState == BuildStates::Upgraded;
}

void ABuilding::HealOverTime()
{
	if (ABaseHex* hex = hexNav->GetCurrentHex())
	{
		if (!hex->battle)
		{
			unitData->HealHP(FMath::RoundToInt(unitData->GetMaxHP() * healPercent));
		}
	}
}

int ABuilding::GetHexLayersToOccupy() const
{
	if (!AGlobalSpawner::spawnerObject->buildingCosts.Contains(buildingType)) return 0;

	return AGlobalSpawner::spawnerObject->buildingCosts[buildingType].hexLayers;
}

