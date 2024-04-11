// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostDefenses.h"

UOutpostDefenses::UOutpostDefenses()
{
	type = BuildingAttachments::DefenseStation;
}

void UOutpostDefenses::UpdateResources()
{
	outpost->unitStats->currentHP += HPIncrease;
	outpost->unitStats->maxHP += HPIncrease;
	outpost->unitStats->siegePower += siegeIncrease;
}

void UOutpostDefenses::DisableAttachment()
{
	if (!outpost) return;

	outpost->unitStats->currentHP -= HPIncrease;
	outpost->unitStats->maxHP -= HPIncrease;
	outpost->unitStats->siegePower -= siegeIncrease;

	Super::DisableAttachment();
}

bool UOutpostDefenses::SetUpAttachment(BuildingAttachments attachment)
{
	if (!outpost->spawner->attachmentStats.Contains(attachment)) return false;

	FBuildingStats stats = outpost->spawner->attachmentStats[attachment];
	FBuildingCost costs = outpost->spawner->attachmentCosts[attachment];

	maxWorkers = stats.maxWorkers;
	buildTime = costs.timeToBuild;
	currBuildTime = costs.timeToBuild;

	HPIncrease = stats.HP;
	siegeIncrease = stats.siegeDamage;

	return true;
}
