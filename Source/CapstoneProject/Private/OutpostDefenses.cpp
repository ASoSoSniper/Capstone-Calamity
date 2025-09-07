// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostDefenses.h"

UOutpostDefenses::UOutpostDefenses()
{
	attachmentType = BuildingAttachments::DefenseStation;
}

void UOutpostDefenses::UpdateResources()
{
	/*outpost->GetUnitData()->currentHP += HPIncrease;
	outpost->GetUnitData()->maxHP += HPIncrease;
	outpost->GetUnitData()->siegePower += siegeIncrease;*/
}

bool UOutpostDefenses::DisableAttachment()
{
	if (!Super::DisableAttachment()) return false;

	/*outpost->GetUnitData()->currentHP -= HPIncrease;
	outpost->GetUnitData()->maxHP -= HPIncrease;
	outpost->GetUnitData()->siegePower -= siegeIncrease;*/

	return true;
}

bool UOutpostDefenses::SetUpAttachment(BuildingAttachments attachment)
{
	if (!AGlobalSpawner::spawnerObject->attachmentStats.Contains(attachment)) return false;

	FBuildingStats stats = AGlobalSpawner::spawnerObject->attachmentStats[attachment];
	FBuildingCost costs = AGlobalSpawner::spawnerObject->attachmentCosts[attachment];

	maxWorkers = stats.maxWorkers;
	buildTime = costs.timeToBuild;
	currBuildTime = costs.timeToBuild;

	HPIncrease = stats.HP;
	siegeIncrease = stats.siegeDamage;

	return true;
}
