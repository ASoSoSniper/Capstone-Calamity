// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_Troop_MoveToPos.h"
#include "Troop.h"

void UUAIA_Troop_MoveToPos::ExecuteAction(IUAI_Controller* controller)
{
	controller->EndAction();
}

void UUAIA_Troop_MoveToPos::SetDestination(IUAI_Controller* controller)
{
	ATroop* troop = Cast<ATroop>(controller);
	if (troop)
	{
		troop->UpdateDestination();
	}
}

bool UUAIA_Troop_MoveToPos::IsMovementAction() const
{
	return true;
}
