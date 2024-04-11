// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostBarracks.h"

UOutpostBarracks::UOutpostBarracks()
{
	type = BuildingAttachments::RobotBarracks;
}

bool UOutpostBarracks::SetUpAttachment(BuildingAttachments attachment)
{
	if (!outpost->spawner->attachmentStats.Contains(attachment)) return false;

	FBuildingStats stats = outpost->spawner->attachmentStats[attachment];
	FBuildingCost costs = outpost->spawner->attachmentCosts[attachment];

	maxWorkers = stats.maxWorkers;
	buildTime = costs.timeToBuild;
	currBuildTime = costs.timeToBuild;

	return true;
}
