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
	unitStats = CreateDefaultSubobject<UUnitStats>(TEXT("Unit Stats"));
	visibility = CreateDefaultSubobject<UMeshVisibility>(TEXT("Mesh Visibility"));
	visibility->enableScan = false;

	buildingType = SpawnableBuildings::None;

	resourceYields.Add(StratResources::Energy, 0);
	resourceYields.Add(StratResources::Food, 0);
	resourceYields.Add(StratResources::Production, 0);
	resourceYields.Add(StratResources::Wealth, 0);
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	SetupBuilding(buildingType);
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SetupBuilding(buildingType)) return;

	switch (buildState)
	{
	case Building:
		Constructing(DeltaTime);
		break;
	case Complete:
		
		break;
	case Destroying:
		DestroyingBuilding(DeltaTime);
		break;
	}
}

bool ABuilding::SetupBuilding(SpawnableBuildings type)
{
	if (unitStats->faction == Factions::None) return false;

	//If both needed objects are found, then setup is complete and this function can be ignored
	if (spawner && hexNav->currentHex) return true;

	//If spawner is not set, search the world for one
	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}
	
	//If the building's hex is not set, scan until a hex is found
	if (!hexNav->currentHex)
	{
		SphereCheck();
	}

	//If either objects are missing, cancel the process
	if (!spawner || !hexNav->currentHex) return false;

	//If data for this building type does not exist, return
	if (type == SpawnableBuildings::None || 
		!spawner->buildingStats.Contains(type)) return false;

	//Get the building data
	FBuildingStats stats = spawner->buildingStats[type];	

	unitStats->currentHP = stats.HP;
	unitStats->maxHP = stats.HP;

	unitStats->damage = stats.siegeDamage;

	unitStats->energyUpkeep = stats.energyUpkeepCost;

	resourceYields[StratResources::Energy] = stats.energyYield;
	resourceYields[StratResources::Food] = stats.foodYield;
	resourceYields[StratResources::Production] = stats.productionYield;
	resourceYields[StratResources::Wealth] = stats.wealthYield;

	FBuildingCost costs;
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
	if (spawner->buildingCosts.Contains(type))
	{
		costs = spawner->buildingCosts[type];

		if (hex->maxWorkers != costs.workerCost)
		{
			hex->maxWorkers = costs.workerCost;
		}
	}

	int hexWorkers = 0;
	int availableWorkers = 0;
	TMap<WorkerType, int> factionWorkers = UnitActions::GetFactionWorkers(unitStats->faction);
	for (auto& worker : hex->workersInHex)
	{
		hexWorkers += worker.Value;
	}
	for (auto& worker : factionWorkers)
	{
		availableWorkers += worker.Value;
	}

	while (hexWorkers < hex->maxWorkers && availableWorkers > 0)
	{
		for (auto& worker : factionWorkers)
		{
			int addedWorker = UnitActions::AddWorkers(unitStats->faction, worker.Key, 1, hex);

			hex->workersInHex[worker.Key] += addedWorker;
			hexWorkers += addedWorker;
			availableWorkers -= addedWorker;

			if (hexWorkers >= hex->maxWorkers || availableWorkers <= 0) break;
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
		break;
	case Complete:
		currBuildTime = upgradeTime;
		break;
	case Uprading:
		buildState = Upgraded;
		break;
	}
}

void ABuilding::UpdateResources()
{
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
	if (!hex) return;

	for (auto& resource : resourceYields)
	{
		hex->UpdateResourceYield(resource.Key, resource.Value);
	}

	UnitActions::UpdateResourceCapacity(unitStats->faction, resourceCapIncrease);
}

void ABuilding::RevertResources()
{
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
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
			if (hexActor && results[i].GetActor() != hexNav->currentHex)
			{
				if (FMath::Abs(GetActorLocation().X - hexActor->GetActorLocation().X) < hexSnapDistance && FMath::Abs(GetActorLocation().Y - hexActor->GetActorLocation().Y) < hexSnapDistance)
				{
					SetActorLocation(hexActor->buildingAnchor->GetComponentLocation());
					hexActor->building = this;
					hexNav->currentHex = hexActor;

					if (hexActor->hexOwner == Factions::None) hexActor->SetFaction(unitStats->faction);

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
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);

	if (hex)
	{
		UnitActions::UpdateResourceCapacity(unitStats->faction, -resourceCapIncrease);

		hex->maxWorkers = 10;
		int totalWorkers = 0;
		for (auto& worker : hex->workersInHex)
		{
			totalWorkers += worker.Value;
		}
		int workersToRemove = totalWorkers > 10 ? totalWorkers - 10 : 0;

		int removedWorkers = 0;
		while (removedWorkers < workersToRemove)
		{
			for (auto& worker : hex->workersInHex)
			{
				if (removedWorkers == workersToRemove) break;

				int remove = UnitActions::RemoveWorkers(unitStats->faction, worker.Key, 1, hex);

				if (remove > 0)
				{
					removedWorkers++;
					hex->workersInHex[worker.Key]--;
				}
			}
		}

		UnitActions::RemoveFromFaction(unitStats->faction, this);

		if (buildingType != SpawnableBuildings::None)
		{
			TMap<StratResources, int> addResources;
			addResources.Add(StratResources::Production, spawner->buildingCosts[buildingType].productionCost * 0.25f);
			UnitActions::AddResources(unitStats->faction, addResources);
		}

		hex->building = nullptr;
	}

	Super::Destroyed();
}

bool ABuilding::IsDisabled()
{
	if (disabled) return true;
	if (unitStats->currentHP > 0) return false;

	disabled = true;
	spawner->SpawnSmoke(this);
	return true;
}

