// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_Branch.h"

void UUAIA_Branch::ExecuteAction(IUAI_Controller* controller)
{
    FUAI_Decision decision;
    decision.bestActionType = GetDefaultActionType();

    decision.bestAction = nullptr;
    decision.bestScore = 0.f;

    for (int i = 0; i < actions.Num(); i++)
    {
        float score = controller->ScoreAction(actions[i]->GetConditions());
        if (score > decision.bestScore)
        {
            decision.bestScore = score;
            decision.bestAction = actions[i];
        }

        FUAI_ActionScore actionScore;
        actionScore.actionName = FText::FromString(actions[i]->GetActionName());
        actionScore.score = score;

        decision.actionScores.Add(actionScore);
    }

    if (decision.bestScore)
    {
        controller->SetBestAction(decision);
    }
}