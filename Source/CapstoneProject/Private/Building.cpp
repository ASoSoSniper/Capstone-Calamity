// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	RootComponent = collider;
	collider->bHiddenInGame = false;
	collider->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	collider->InitBoxExtent(FVector(20.f, 20.f, 20.f));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));	
	UStaticMesh* meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/Meshes/Box/Box.Box'"));
	if (meshAsset)
	{
		mesh->SetStaticMesh(meshAsset);
	}
	mesh->SetupAttachment(RootComponent);

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
	}
}

void ABuilding::Constructing(float& DeltaTime)
{
	currBuildTime -= DeltaTime;
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

