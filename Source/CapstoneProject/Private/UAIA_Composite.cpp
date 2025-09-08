// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_Composite.h"

void UUAIA_Composite::ExecuteAction(IUAI_Controller* controller)
{
	if (actions.IsEmpty())
	{
		controller->EndAction();
		return;
	}

	controller->SetCompositeAction(this);

	actions[controller->GetCompositeActionIndex()]->ExecuteAction(controller);

	if (!controller->AdvanceCompositeActionIndex(actions.Num()))
		controller->ResetCompositeAction();
}

bool UUAIA_Composite::IsMovementAction(IUAI_Controller* controller) const
{
	return actions[controller->GetCompositeActionIndex()]->IsMovementAction();
}

void UUAIA_Composite::SetDestination(IUAI_Controller* controller)
{
	if (actions.IsEmpty()) return;

	actions[controller->GetCompositeActionIndex()]->SetDestination(controller);
}

bool UUAIA_Composite::CanAbandon(IUAI_Controller* controller) const
{
	if (actions.IsEmpty()) return false;

	return actions[controller->GetCompositeActionIndex()]->CanAbandon();
}

float UUAIA_Composite::GetAbandonTime(IUAI_Controller* controller) const
{
	if (actions.IsEmpty()) return 0.f;

	return actions[controller->GetCompositeActionIndex()]->GetAbandonTime();
}

bool UUAIA_Composite::CanUpdateDestination(IUAI_Controller* controller) const
{
	if (actions.IsEmpty()) return false;

	return actions[controller->GetCompositeActionIndex()]->CanUpdateDestination();
}
