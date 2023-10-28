// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "CapstoneProjectGameModeBase.h"

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
		mesh->SetStaticMesh(meshAsset);
	}
	mesh->SetCollisionProfileName("NoCollision");

	collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	collider->SetupAttachment(RootComponent);
	collider->bHiddenInGame = false;
	collider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	collider->InitBoxExtent(FVector(20.f, 20.f, 20.f));
	collider->SetRelativeLocation(FVector(0.f, 0.f, 20.f));

	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interaction"));
	hexNav = CreateDefaultSubobject<UHexNav>(TEXT("Hex Nav"));
	unitStats = CreateDefaultSubobject<UUnitStats>(TEXT("Unit Stats"));
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
		
	SetBuildState();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Begun building"));
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (buildState)
	{
	case Building:
		Constructing(DeltaTime);
		break;
	case Complete:
		//Harvest(DeltaTime);
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

