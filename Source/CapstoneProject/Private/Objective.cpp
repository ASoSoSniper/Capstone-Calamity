// Fill out your copyright notice in the Description page of Project Settings.


#include "Objective.h"
#include "Faction.h"
#include "Reward.h"
#include "EventSystemManager.h"

FString UObjective::GetObjectiveTitle() const
{
	return title;
}

FString UObjective::GetObjectiveDescription() const
{
	return description;
}

const TArray<UReward*>& UObjective::GetRewards() const
{
	return rewards;
}

FString UObjective::GetObjectiveDisplay() const
{
	return GetObjectiveDescription() + TEXT(": 0 / 1");
}

void UObjective::SetupObjective(UFaction* faction)
{
	factionObject = faction;
	//faction->onTroopBuilt.AddDynamic(this, UObjective::HandleUpdate);
}

/*void HandleUpdate(ATroop* troop)
{
	if (troop->unitType == UnitTypes::Settler)
	{
		settlersBuilt++;
		if (settlersBuilt == countToComplete)
		{
			CompleteObjective();
		}
	}
}*/

void UObjective::CompleteObjective()
{
	for(UReward* reward : rewards)
	{
		reward->ApplyReward(factionObject);
	}
	onObjectiveComplete.Broadcast(this);
	CleanupObjective();
}

void UObjective::CleanupObjective()
{
	onObjectiveComplete.Clear();
	factionObject = nullptr;
}
