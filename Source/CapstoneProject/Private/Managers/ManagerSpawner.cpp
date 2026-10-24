// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/ManagerSpawner.h"

// Sets default values
AManagerSpawner::AManagerSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AManagerSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AManagerSpawner::SpawnManagers);
}

void AManagerSpawner::SpawnManagers()
{
	for (int i = 0; i < managers.Num(); i++)
	{
		GetWorld()->SpawnActor(managers[i]);
	}
}

