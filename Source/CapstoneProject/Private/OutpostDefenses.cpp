// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostDefenses.h"

UOutpostDefenses::UOutpostDefenses()
{
	type = BuildingAttachments::DefenseStation;
}

void UOutpostDefenses::UpdateResources()
{
	outpost->unitStats->currentHP += HPInrease;
	outpost->unitStats->maxHP += HPInrease;
	outpost->unitStats->siegePower += siegeIncrease;
}

void UOutpostDefenses::DisableAttachment()
{
	if (!outpost) return;

	outpost->unitStats->currentHP -= HPInrease;
	outpost->unitStats->maxHP -= HPInrease;
	outpost->unitStats->siegePower -= siegeIncrease;

	Super::DisableAttachment();
}
