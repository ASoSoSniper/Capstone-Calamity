// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostStorage.h"

void UOutpostStorage::UpdateResources()
{
	UnitActions::UpdateResourceCapacity(outpost->unitStats->faction, storageIncrease);
}

UOutpostStorage::UOutpostStorage()
{
	type = BuildingAttachments::Storage;
}

void UOutpostStorage::ActivateAttachment()
{
	Super::ActivateAttachment();
}

bool UOutpostStorage::DisableAttachment()
{
	if (!Super::DisableAttachment()) return false;

	UnitActions::UpdateResourceCapacity(outpost->unitStats->faction, -storageIncrease);
	return true;
}

bool UOutpostStorage::SetUpAttachment(BuildingAttachments attachment)
{
	if (!outpost->spawner->attachmentStats.Contains(attachment)) return false;

	FBuildingStats stats = outpost->spawner->attachmentStats[attachment];
	FBuildingCost costs = outpost->spawner->attachmentCosts[attachment];

	maxWorkers = stats.maxWorkers;
	buildTime = costs.timeToBuild;
	currBuildTime = costs.timeToBuild;

	storageIncrease = stats.resourceCapIncrease;

	return true;
}
