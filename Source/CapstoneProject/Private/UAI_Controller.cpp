// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_Controller.h"
#include "UAI_Action.h"
#include "UAI_Condition.h"
#include "CapstoneProjectGameModeBase.h"

UAI_Action* IUAI_Controller::GetBestAction() const
{
    return bestAction;
}

void IUAI_Controller::SetBestAction(UAI_Action* action, EActionType actionType)
{
    if (!action) return;

    bestAction = action;
    currentActionType = actionType != EActionType::None ? actionType : action->GetDefaultActionType();

    if (bestAction->CanAbandon(this)) abandonTime = bestAction->GetAbandonTime(this);

    if (bestAction->IsMovementAction(this))
    {
        bestAction->SetDestination(this);
        FSM_State = DestinationReached() ? EDecisionState::Executing : EDecisionState::Moving;
    }
    else
        FSM_State = EDecisionState::Executing;
}

void IUAI_Controller::EndAction()
{
    FSM_Reset();
}

EActionType IUAI_Controller::DecideBestActionType(TMap<EActionType, FActionSelection>& actionTypes)
{
    EActionType selectedAction = EActionType::None;
    float bestScore = GetDefaultScore();

    for (TPair<EActionType, FActionSelection>& action : actionTypes)
    {
        float score = ScoreAction(action.Value.conditions);
        if (score > bestScore)
        {
            bestScore = score;
            selectedAction = action.Key;
        }
    }

    return selectedAction;
}

bool IUAI_Controller::DecideBestAction()
{
    if (compositeAction)
    {
        SetBestAction(compositeAction);
        return true;
    }

    TMap<EActionType, FActionSelection>& actionTypes = GetActions();
    EActionType selectedType = DecideBestActionType(actionTypes);

    UAI_Action* selectedAction = nullptr;
    float bestScore = GetDefaultScore();

    for (int i = 0; i < actionTypes[selectedType].actions.Num(); i++)
    {
        float score = ScoreAction(actionTypes[selectedType].actions[i]->GetConditions());
        if (score > bestScore)
        {
            bestScore = score;
            selectedAction = actionTypes[selectedType].actions[i];
        }
    }

    SetBestAction(selectedAction);
    return selectedAction != nullptr;
}

float IUAI_Controller::ScoreAction(const TArray<UAI_Condition*>& conditions)
{
    float score = 1.f;

    if (conditions.IsEmpty()) return 1.f;

    for (int i = 0; i < conditions.Num(); i++)
    {
        score *= conditions[i]->ScoreCondition(this);

        if (score == 0) return 0;
    }

    float originalScore = score;
    float modFactor = 1 - (1 / conditions.Num());
    float makeUpValue = (1 - originalScore) * modFactor;
    return originalScore + (makeUpValue * originalScore);
}

bool IUAI_Controller::UpdateTimer(const float& DeltaTime)
{
    updateTime -= DeltaTime;
    if (updateTime > 0.f) return false;

    updateTime = GetUpdateRate();
    return true;
}

bool IUAI_Controller::AbandonTimer(const float& DeltaTime)
{
    if (!bestAction) return true;
    if (!bestAction->CanAbandon(this)) return false;

    abandonTime -= DeltaTime;
    if (abandonTime > 0.f) return false;

    return true;
}

void IUAI_Controller::SetDestinationUpdateTimer(const float& duration)
{
    if (!bestAction->CanUpdateDestination(this)) return;

    destinationUpdateTime = duration;
}

bool IUAI_Controller::DestinationUpdateTimer(const float& DeltaTime)
{
    if (!bestAction->CanUpdateDestination(this)) return false;

    destinationUpdateTime -= DeltaTime;
    if (destinationUpdateTime > 0.f) return false;

    return true;
}

void IUAI_Controller::FSM_Tick(const float& DeltaTime)
{
    float scaledDeltaTime = DeltaTime * ACapstoneProjectGameModeBase::timeScale;

    switch (FSM_State)
    {
    case EDecisionState::Deciding:
        if (!IsAIControlled() && !compositeAction) return;
        if (UpdateTimer(scaledDeltaTime))
            DecideBestAction();
        break;
    case EDecisionState::Moving:
        if (AbandonTimer(scaledDeltaTime))
        {
            FSM_Reset(); 
            ResetCompositeAction();
        }
        if (DestinationUpdateTimer(scaledDeltaTime))
            bestAction->SetDestination(this);
        if (DestinationReached())
            FSM_State = EDecisionState::Executing;
        break;
    case EDecisionState::Executing:
        if (bestAction)
        {
            bestAction->ExecuteAction(this);
            bestAction = nullptr;
        }
        break;
    }
}

void IUAI_Controller::FSM_Reset()
{
    bestAction = nullptr;
    FSM_State = EDecisionState::Deciding;
}

bool IUAI_Controller::IsAIControlled()
{
    return false;
}

float IUAI_Controller::GetDefaultScore() const
{
    return 0.0f;
}

float IUAI_Controller::GetUpdateRate() const
{
    return 0.0f;
}

void IUAI_Controller::SetCompositeAction(UAI_Action* action)
{
    compositeAction = action;
}

void IUAI_Controller::ResetCompositeAction()
{
    compositeAction = nullptr;
    compositeActionIndex = 0;
}

int IUAI_Controller::GetCompositeActionIndex() const
{
    return compositeActionIndex;
}

bool IUAI_Controller::AdvanceCompositeActionIndex(int maxLength)
{
    compositeActionIndex++;

    return compositeActionIndex < maxLength;
}
