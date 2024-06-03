// Fill out your copyright notice in the Description page of Project Settings.


#include "Faction.h"
#include "CapstoneProjectGameModeBase.h"


Faction::Faction()
{
	faction = Factions::None;
	resourceInventory.Add(StratResources::Energy, InventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(StratResources::Production, InventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(StratResources::Food, InventoryStat{ 300,300,0, 0 });
	resourceInventory.Add(StratResources::Wealth, InventoryStat{ 000, 300, 0, 0 });

	availableWorkers.Add(WorkerType::Human, WorkerStats{ 0,500, 0,1,0 });
	availableWorkers.Add(WorkerType::Robot, WorkerStats{ 0,10, 1,0,0 });
	availableWorkers.Add(WorkerType::Alien, WorkerStats{ 0,0, 0,1,0 });

	armyNamesHuman.Add(TEXT("Fuckers"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Asswipes"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Shitstains"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Muppets"), TArray<int32>());
	armyNamesHuman.Add(TEXT("Dummies"), TArray<int32>());
	armyNamesHuman.Add(TEXT("NPCs"), TArray<int32>());

	armyNamesAlien.Add(TEXT("Uncrustables"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Oatmeal Cream Pies"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Hot Pockets"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Pop Tarts"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Quakers"), TArray<int32>());
	armyNamesAlien.Add(TEXT("Lucky Charms"), TArray<int32>());
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
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Faction found %d other factions!"), factionRelationships.Num()));
}
