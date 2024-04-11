// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostTroopFactory.h"

UOutpostTroopFactory::UOutpostTroopFactory()
{
	type = BuildingAttachments::RobotFactory;
}

bool UOutpostTroopFactory::SetUpAttachment(BuildingAttachments attachment)
{
	if (!outpost->spawner->attachmentStats.Contains(attachment)) return false;

	FBuildingStats stats = outpost->spawner->attachmentStats[attachment];
	FBuildingCost costs = outpost->spawner->attachmentCosts[attachment];

	maxWorkers = stats.maxWorkers;
	buildTime = costs.timeToBuild;
	currBuildTime = costs.timeToBuild;

	return true;
}
