// Fill out your copyright notice in the Description page of Project Settings.


#include "Faction.h"
#include "CapstoneProjectGameModeBase.h"


Faction::Faction()
{
	faction = Factions::None;
	resourceInventory.Add(StratResources::Energy, InventoryStat{ 100,100,0 });
	resourceInventory.Add(StratResources::Production, InventoryStat{ 100,100,0 });
	resourceInventory.Add(StratResources::Food, InventoryStat{ 100,100,0 });

	availableWorkers.Add(WorkerType::Human, WorkerStats{ 0,10 });
	availableWorkers.Add(WorkerType::Robot, WorkerStats{ 0,100 });
	availableWorkers.Add(WorkerType::Alien, WorkerStats{ 0,0 });
}

Faction::~Faction()
{
}

void Faction::FindActiveFactions()
{
	for (auto currentFaction : ACapstoneProjectGameModeBase::activeFactions)
	{
		if (currentFaction.Key != faction)
		{
			FactionRelationship relationShip = FactionRelationship::Neutral;

			factionRelationships.Add(currentFaction.Key, relationShip);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Faction found %d other factions!"), factionRelationships.Num()));
}
