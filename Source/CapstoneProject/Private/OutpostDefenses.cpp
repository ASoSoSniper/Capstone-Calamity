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

bool UOutpostDefenses::DisableAttachment()
{
	if (!Super::DisableAttachment()) return false;

	outpost->unitStats->currentHP -= HPIncrease;
	outpost->unitStats->maxHP -= HPIncrease;
	outpost->unitStats->siegePower -= siegeIncrease;

	return true;
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
