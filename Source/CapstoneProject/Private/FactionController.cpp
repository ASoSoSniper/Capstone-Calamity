// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionController.h"
#include "CapstoneProjectGameModeBase.h"

// Sets default values
AFactionController::AFactionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFactionController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFactionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DateUpdateTick();
	FSM_Tick(DeltaTime);
}

void AFactionController::SetFaction(Faction* setFaction)
{
	if (faction != nullptr) return;

	faction = setFaction;
}

bool AFactionController::IsAIControlled()
{
	return faction->GetFaction() != Factions::Human;
}

float AFactionController::GetUpdateRate() const
{
	return ACapstoneProjectGameModeBase::GetTimeTillNextTick();
}

void AFactionController::DateUpdateTick()
{
	FDateTickUpdate* update = ACapstoneProjectGameModeBase::GetDateUpdates();

	if (update->minuteTick)
	{
		
	}
	if (update->hourTick)
	{
		faction->UpdateResourceCosts();
	}
	if (update->dayTick)
	{
		faction->FeedPop();
		faction->ConsumeEnergy();
	}
	if (update->monthTick)
	{

	}
}

