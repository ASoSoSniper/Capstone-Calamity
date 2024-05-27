// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "CapstoneProjectGameModeBase.h"

// Sets default values
AFloor::AFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	interactable = CreateDefaultSubobject<UInteractable>(TEXT("Interactable"));
	interactable->canInteract = false;
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();

	if (!material) material = mesh->GetMaterial(0);
	
	floorMaterial = UMaterialInstanceDynamic::Create(material, this);
	
	mesh->SetMaterial(0, floorMaterial);

	floorMaterial->SetScalarParameterValue(FName("WorldTime"), worldTime);
}

// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (disableSpeedChange) return;

	worldTime += DeltaTime * ACapstoneProjectGameModeBase::timeScale;

	floorMaterial->SetScalarParameterValue(FName("WorldTime"), worldTime);
}

