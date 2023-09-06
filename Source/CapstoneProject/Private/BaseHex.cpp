// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHex.h"


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
	
	hexInfo = CreateDefaultSubobject<UHexInfo>(TEXT("Hex Info"));
	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));

	collider = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	collider->SetupAttachment(RootComponent);
	collider->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	troopAnchor = CreateDefaultSubobject<USceneComponent>("Troop Anchor");
	troopAnchor->SetupAttachment(RootComponent);

	buildingAnchor = CreateDefaultSubobject<USceneComponent>("Building Anchor");
	buildingAnchor->SetupAttachment(RootComponent);
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

}

