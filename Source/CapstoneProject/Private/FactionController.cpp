// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionController.h"
#include "CapstoneProjectGameModeBase.h"
#include "Faction.h"

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

	if (!faction) return;

	DateUpdateTick();
	FSM_Tick(DeltaTime);
}

void AFactionController::SetFaction(UFaction* setFaction)
{
	if (faction != nullptr) return;

	faction = setFaction;
}

UFaction* AFactionController::GetFactionObject() const
{
	return faction;
}

bool AFactionController::IsAIControlled()
{
	if (!faction) return false;

	return faction->GetFaction() != Factions::Human;
}

void AFactionController::TriggerUpdateDisplay()
{
	updateDecisionDisplay = true;
}

float AFactionController::GetUpdateRate() const
{
	return ACapstoneProjectGameModeBase::GetTimeTillNextTick() * updateTime;
}

void AFactionController::SetBestAction(FUAI_Decision& decision)
{
	IUAI_Controller::SetBestAction(decision);

	updateDecisionDisplay = true;
}

const TMap<EActionType, FActionSelection>& AFactionController::GetActions()
{
	return actions;
}

bool AFactionController::DestinationReached() const
{
	return false;
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

bool AFactionController::UpdateDisplay()
{
	if (updateDecisionDisplay)
	{ 
		updateDecisionDisplay = false;
		return true;
	}

	return false;
}

bool AFactionController::DecisionsMade() const
{
	return !GetDecisionHistory().IsEmpty();
}

const FUAI_Decision& AFactionController::GetDecisionFromHistory(int index) const
{
	const TArray<FUAI_Decision>& history = GetDecisionHistory();

	index = FMath::Clamp(index, 0, history.Num() - 1);

	return history[index];
}

FText AFactionController::GetActionFromHistory(int decision, int index) const
{
	const FUAI_Decision& fDecision = GetDecisionFromHistory(decision);
	if (index >= fDecision.actionScores.Num()) return FText::FromString("No Action");
	FUAI_ActionScore action = fDecision.actionScores[index];

	FString actionInfo = FString::Printf(TEXT("%s: %.2f"), *action.actionName.ToString(), action.score);

	return FText::FromString(actionInfo);

}

FText AFactionController::GetActionUpdateCountdown() const
{
	return FText::FromString(FString::Printf(TEXT("Next Decision: %.2f"), GetTimeTillNextDecision()));
}

FText AFactionController::GetActionAbandonCountdown() const
{
	return FText::FromString(FString::Printf(TEXT("Abandon Action In: %.2f"), GetTimeTIllActionAbandon()));
}