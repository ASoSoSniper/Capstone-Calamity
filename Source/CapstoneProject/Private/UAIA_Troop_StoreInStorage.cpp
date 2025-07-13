// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_Troop_StoreInStorage.h"
#include "Troop.h"
#include "Outpost.h"

void UUAIA_Troop_StoreInStorage::ExecuteAction(IUAI_Controller* controller)
{
	ATroop* troop = Cast<ATroop>(controller);
	if (!troop) return;

	UHexNav* targetNav = troop->GetTargetUnit();
	if (!targetNav) return;

	AOutpost* outpost = Cast<AOutpost>(targetNav->GetOwner());
	if (!outpost) return;

	outpost->StoreTroop(troop);
}
