// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_Troop_Merge.h"
#include "GlobalSpawner.h"
#include "Troop.h"

void UUAIA_Troop_Merge::ExecuteAction(IUAI_Controller* controller)
{
	ATroop* troop = Cast<ATroop>(controller);
	if (!troop) return;

	UHexNav* targetNav = troop->GetTargetUnit();
	if (!targetNav) return;

	ATroop* target = Cast<ATroop>(targetNav->GetOwner());
	if (!target) return;

	if (troop == target) return;

	if (AGlobalSpawner::spawnerObject)
		AGlobalSpawner::spawnerObject->MergeArmies(troop, target, troop->hexNav->GetCurrentHex());
}
