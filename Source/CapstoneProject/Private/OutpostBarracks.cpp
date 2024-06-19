// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostBarracks.h"

UOutpostBarracks::UOutpostBarracks()
{
	type = BuildingAttachments::RobotBarracks;
}

bool UOutpostBarracks::SetUpAttachment(BuildingAttachments attachment)
{
	if (!AGlobalSpawner::spawnerObject->attachmentStats.Contains(attachment)) return false;

	FBuildingStats stats = AGlobalSpawner::spawnerObject->attachmentStats[attachment];
	FBuildingCost costs = AGlobalSpawner::spawnerObject->attachmentCosts[attachment];

	maxWorkers = stats.maxWorkers;
	buildTime = costs.timeToBuild;
	currBuildTime = costs.timeToBuild;

	return true;
}
