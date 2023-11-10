// Copyright Epic Games, Inc. All Rights Reserved.


#include "CapstoneProjectGameModeBase.h"

ACapstoneProjectGameModeBase::ACapstoneProjectGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
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
			activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources += faction.Value->resourceInventory[resource.Key].resourcePerTick;

			activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources = FMath::Clamp(activeFactions[faction.Key]->resourceInventory[resource.Key].currentResources, 0, activeFactions[faction.Key]->resourceInventory[resource.Key].maxResources);
		}
	}
	currentHarvestTime = harvestTickLength;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Food = %d, %d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Food].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Food].resourcePerTick));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Production = %d, %d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Production].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Production].resourcePerTick));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("Energy = %d, %d per tick"), activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].currentResources, activeFactions[Factions::Human]->resourceInventory[StratResources::Energy].resourcePerTick));
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
