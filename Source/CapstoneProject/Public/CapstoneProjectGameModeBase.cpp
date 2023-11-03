// Copyright Epic Games, Inc. All Rights Reserved.


#include "CapstoneProjectGameModeBase.h"

ACapstoneProjectGameModeBase::ACapstoneProjectGameModeBase()
{
	
}

void ACapstoneProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//Create initial player faction and assign it to the player controller
	CreateNewFaction();

	//Create alien factions, quantity determined by variable
	for (int i = 0; i < alienFactionQuantity; ++i)
	{
		CreateNewFaction();
	}

	for (auto currentFaction : activeFactions)
	{
		currentFaction.Value->FindActiveFactions();
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::Printf(TEXT("%d factions created!"), activeFactions.Num()));

	//For debug purposes, Human and Alien1 hate each other, Human and Alien2 are chill, and Alien3+ literally couldn't give less of a fuck
	activeFactions[Factions::Human]->factionRelationships[Factions::Alien1] = FactionRelationship::Enemy;
	activeFactions[Factions::Alien1]->factionRelationships[Factions::Human] = FactionRelationship::Enemy;

	activeFactions[Factions::Human]->factionRelationships[Factions::Alien2] = FactionRelationship::Ally;
	activeFactions[Factions::Alien2]->factionRelationships[Factions::Human] = FactionRelationship::Ally;

	FActorSpawnParameters params;
	GetWorld()->SpawnActor<AGlobalSpawner>(spawner, params);
}

void ACapstoneProjectGameModeBase::Tick(float DeltaTime)
{
	if (currentHarvestTime > 0)
	{
		currentHarvestTime -= DeltaTime * timeScale;
		return;
	}

	for (auto faction : activeFactions)
	{
		for (auto resource : faction.Value->resourceInventory)
		{
			resource.Value += faction.Value->resourcePerTick[resource.Key];
		}
	}
	currentHarvestTime = harvestTickLength;
}

Factions ACapstoneProjectGameModeBase::CreateNewFaction()
{
	//Advance in the Factions enum, element 0 is None
	++factionCount;
	
	//Get the selected faction in the Factions enum
	Factions selectedFaction = static_cast<Factions>(factionCount);

	//Create a new Faction instance
	Faction* newFaction = new Faction();

	//Make the new Faction instance aware of the faction it's assigned to
	newFaction->faction = selectedFaction;

	//In the global faction dictionary, add the selected faction as a new key and the Faction instance as its value
	activeFactions.Add(selectedFaction, newFaction);

	//Return the selected faction
	return selectedFaction;
}
