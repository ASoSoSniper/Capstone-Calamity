// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSystemManager.h"
#include "UnitActions.h"

AEventSystemManager* AEventSystemManager::eventManager = nullptr;

// Sets default values
AEventSystemManager::AEventSystemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEventSystemManager::BeginPlay()
{
	Super::BeginPlay();
	
	eventManager = this;
	playerFaction = UnitActions::GetFaction(EFactions::Human);
}

// Called every frame
void AEventSystemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEventSystemManager::TriggerEvent(FWorldEvent event)
{
	eventManager->activeEvent = &event;
}

void AEventSystemManager::CompleteObjective(UObjective* objective)
{
	eventManager->dockedObjectives.Remove(objective);
}

void AEventSystemManager::CloseActiveEvent()
{
	activeEvent = nullptr;
	//UI logic
}

void AEventSystemManager::SelectOption(FEventOption option)
{
	for (UObjective* objective : option.objectives)
	{
		dockedObjectives.Add(objective);
		objective->onObjectiveComplete.AddDynamic(this, &AEventSystemManager::CompleteObjective);
		objective->SetupObjective(playerFaction);
	}
}
