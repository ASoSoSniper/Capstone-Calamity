// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSpawner.h"

// Sets default values
AEventSpawner::AEventSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEventSpawner::SpawnEvent(float deltaTime)
{
}

// Called when the game starts or when spawned
void AEventSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	//eventWidget = CreateWidget<UUserWidget>(GetWorld(), eventPrefab, TEXT("CurrEvent"));
	//eventWidget->AddToViewport();
}

// Called every frame
void AEventSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

