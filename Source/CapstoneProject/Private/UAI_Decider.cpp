// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_Decider.h"
#include "CapstoneProjectGameModeBase.h"
#include "Faction.h"

// Sets default values for this component's properties
UUAI_Decider::UUAI_Decider()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUAI_Decider::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UUAI_Decider::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!faction) return;
	FSM_Tick(DeltaTime);
}

bool UUAI_Decider::IsAIControlled()
{
	return faction && faction->IsAIControlled();
}

void UUAI_Decider::Initialize(UFaction* inFaction)
{
	faction = inFaction;
}

float UUAI_Decider::GetUpdateRate() const
{
	return ACapstoneProjectGameModeBase::GetTimeTillNextTick() * updateTime;
}

void UUAI_Decider::SetBestAction(FUAI_Decision& decision)
{
	IUAI_Controller::SetBestAction(decision);

	onDecisionMade.Broadcast();
}

const TMap<EActionType, FActionSelection>& UUAI_Decider::GetActions()
{
	return actions;
}

bool UUAI_Decider::DestinationReached() const
{
	return false;
}

const FUAI_Decision& UUAI_Decider::GetDecisionFromHistory(int index) const
{
	const TArray<FUAI_Decision>& history = GetDecisionHistory();

	index = FMath::Clamp(index, 0, history.Num() - 1);

	return history[index];
}

FText UUAI_Decider::GetActionFromHistory(int decision, int index) const
{
	const FUAI_Decision& fDecision = GetDecisionFromHistory(decision);
	if (index >= fDecision.actionScores.Num()) return FText::FromString("No Action");
	FUAI_ActionScore action = fDecision.actionScores[index];

	FString actionInfo = FString::Printf(TEXT("%s: %.2f"), *action.actionName.ToString(), action.score);

	return FText::FromString(actionInfo);

}

FText UUAI_Decider::GetActionUpdateCountdown() const
{
	return FText::FromString(FString::Printf(TEXT("Next Decision: %.2f"), GetTimeTillNextDecision()));
}

FText UUAI_Decider::GetActionAbandonCountdown() const
{
	return FText::FromString(FString::Printf(TEXT("Abandon Action In: %.2f"), GetTimeTIllActionAbandon()));
}

bool UUAI_Decider::DecisionsMade() const
{
	return !GetDecisionHistory().IsEmpty();
}
