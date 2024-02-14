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
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/CysAwfulBuilding.CysAwfulBuilding'"));
	if (meshAsset)
	{
		//mesh->SetStaticMesh(meshAsset);
	}
	mesh->SetCollisionProfileName("NoCollision");
	mesh->SetCollisionObjectType(ECC_GameTraceChannel1);

	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interaction"));
	hexNav = CreateDefaultSubobject<UHexNav>(TEXT("Hex Nav"));
	unitStats = CreateDefaultSubobject<UUnitStats>(TEXT("Unit Stats"));
	visibility = CreateDefaultSubobject<UMeshVisibility>(TEXT("Mesh Visibility"));
	visibility->enableScan = false;

	buildingType = SpawnableBuildings::None;
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCheck();
	SetBuildState();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Begun building"));

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);

		if (ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex))
		{
			if (hex->maxWorkers != spawner->buildingCosts[buildingType].workerCost)
			{
				hex->maxWorkers = spawner->buildingCosts[buildingType].workerCost;
			}
		}
	}
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);

		if (ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex))
		{
			if (hex->maxWorkers != spawner->buildingCosts[buildingType].workerCost)
			{
				hex->maxWorkers = spawner->buildingCosts[buildingType].workerCost;
			}
		}
	}

	switch (buildState)
	{
	case Building:
		Constructing(DeltaTime);
		break;
	case Complete:
		//Harvest(DeltaTime);
		break;
	case Destroying:
		DestroyingBuilding(DeltaTime);
		break;
	}
}

void ABuilding::Constructing(float& DeltaTime)
{
	currBuildTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
	if (currBuildTime <= 0.f)
	{
		SetBuildState();
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Build complete!"));
	}
}

void ABuilding::SetBuildState()
{
	switch (buildState)
	{
	case None:
		currBuildTime = buildTime;
		buildState = Building;
		break;
	case Building:
		buildState = Complete;
		UpdateResources();
		BuildingAction();
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

void ABuilding::Harvest(ABaseHex* hex)
{
}

void ABuilding::UpdateResources()
{
}

void ABuilding::RevertResources()
{
}

void ABuilding::BuildingAction()
{
}

void ABuilding::SphereCheck()
{
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	TArray<FHitResult> results;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetActorLocation(), GetActorLocation(), 20.f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::ForOneFrame, results, true);

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

					if (hexActor->hexOwner == Factions::None) hexActor->hexOwner = unitStats->faction;

					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Snapped to target"));
					break;
				}
			}
		}
	}
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
	if (currBuildTime > 0.f)
	{
		currBuildTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		return;
	}

	Destroy();
}

void ABuilding::Destroyed()
{
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
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

	Super::Destroyed();
}

