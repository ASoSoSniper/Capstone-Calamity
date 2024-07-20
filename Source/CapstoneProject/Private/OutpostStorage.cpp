// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostStorage.h"

void UOutpostStorage::UpdateResources()
{
	UnitActions::UpdateResourceCapacity(outpost->unitStats->faction, storageIncrease);
}

UOutpostStorage::UOutpostStorage()
{
	attachmentType = BuildingAttachments::Storage;
}

void UOutpostStorage::ActivateAttachment(bool instantBuild)
{
	Super::ActivateAttachment(instantBuild);
}

bool UOutpostStorage::DisableAttachment()
{
	if (!Super::DisableAttachment()) return false;

	UnitActions::UpdateResourceCapacity(outpost->unitStats->faction, -storageIncrease);
	return true;
}

bool UOutpostStorage::SetUpAttachment(BuildingAttachments attachment)
{
	if (!AGlobalSpawner::spawnerObject->attachmentStats.Contains(attachment)) return false;

	FBuildingStats stats = AGlobalSpawner::spawnerObject->attachmentStats[attachment];
	FBuildingCost costs = AGlobalSpawner::spawnerObject->attachmentCosts[attachment];

	maxWorkers = stats.maxWorkers;
	buildTime = costs.timeToBuild;
	currBuildTime = costs.timeToBuild;

	storageIncrease = stats.resourceCapIncrease;

	return true;
}
