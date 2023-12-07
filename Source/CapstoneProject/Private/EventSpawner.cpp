// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSpawner.h"
#include "Kismet/GameplayStatics.h"

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
	
	/*controller = GetWorld()->GetFirstPlayerController();

	if (eventPrefab)
	{
		//eventWidget = CreateWidget<UUserWidget>(controller, eventPrefab, TEXT("CurrEvent"));
		//eventWidget->AddToViewport();
	}*/
}

// Called every frame
void AEventSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

