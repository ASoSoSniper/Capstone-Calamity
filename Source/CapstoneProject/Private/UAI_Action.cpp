// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_Action.h"

TArray<UAI_Condition*>& UAI_Action::GetConditions()
{
    return conditions;
}

EActionType UAI_Action::GetDefaultActionType() const
{
    return defaultActionType;
}

const FString UAI_Action::GetActionName() const
{
    return actionName;
}

bool UAI_Action::IsMovementAction(IUAI_Controller* controller) const
{
    return false;
}

void UAI_Action::SetDestination(IUAI_Controller* controller)
{
}

bool UAI_Action::CanAbandon(IUAI_Controller* controller) const
{
    return canAbandon;
}

float UAI_Action::GetAbandonTime(IUAI_Controller* controller) const
{
    return abandonTime;
}

bool UAI_Action::CanUpdateDestination(IUAI_Controller* controller) const
{
    return updateDestination;
}
