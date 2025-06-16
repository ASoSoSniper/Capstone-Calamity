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

bool UAI_Action::IsMovementAction() const
{
    return bMovementAction;
}

void UAI_Action::SetDestination(IUAI_Controller* controller)
{
}

bool UAI_Action::CanAbandon() const
{
    return canAbandon;
}

float UAI_Action::GetAbandonTime() const
{
    return abandonTime;
}
