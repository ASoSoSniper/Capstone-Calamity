// Fill out your copyright notice in the Description page of Project Settings.


#include "EventSystemManager.h"
#include "UnitActions.h"

AEventSystemManager* AEventSystemManager::eventManager = nullptr;
const FString AEventSystemManager::eventSearchContext = TEXT("Empty Context");

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

void AEventSystemManager::TriggerEvent(FName eventKey)
{
	FWorldEvent* event = GetEvent(eventKey);
	if (!event) return;

	if (eventManager->activeEvent) eventManager->queuedEvents.Enqueue(eventKey);
	else 
	{
		eventManager->activeEvent = event;
		eventManager->onEventTriggered.Broadcast(*eventManager->activeEvent);
	}
}

FWorldEvent* AEventSystemManager::GetEvent(FName eventKey)
{
	if (!eventManager || !eventManager->eventTable) return nullptr;

	FWorldEvent* event = eventManager->eventTable->FindRow<FWorldEvent>(eventKey, eventSearchContext);

	return event;
}

void AEventSystemManager::CompleteObjective(UObjective* objective)
{
	eventManager->dockedObjectives.Remove(objective);
}

void AEventSystemManager::CloseActiveEvent()
{
	onEventClosed.Broadcast();
	activeEvent = nullptr;

	if (!queuedEvents.IsEmpty())
	{
		FName eventKey;
		queuedEvents.Dequeue(eventKey);

		TriggerEvent(eventKey);
	}
}

void AEventSystemManager::SelectOption(const FEventOption& option)
{
	for (UObjective* objective : option.objectives)
	{
		dockedObjectives.Add(objective);
		objective->onObjectiveComplete.AddDynamic(this, &AEventSystemManager::CompleteObjective);
		objective->SetupObjective(playerFaction);
	}
}
