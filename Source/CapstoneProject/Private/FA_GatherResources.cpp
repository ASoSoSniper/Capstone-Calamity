// Fill out your copyright notice in the Description page of Project Settings.


#include "FA_GatherResources.h"
#include "GlobalSpawner.h"

/*
    --Gather Resources (Via Workers)-- 
    
    Score Factors:
        - Resource quantities are below the defined 'low' current/max percentage
            - Yields higher scores the fewer resources are available
            - Scores each resource separately and adds them to make the combined total
    Action:
        - Fill an owned tile with workers
            - Selects tile with a high output of the desired resource
            - (Not implemented) Selects tile in close proximity to an outpost
*/

FA_GatherResources::FA_GatherResources()
{
}

FA_GatherResources::~FA_GatherResources()
{
}

int FA_GatherResources::ScoreAction(Faction_AIController& controller)
{
    int lowResources = 
        LowResources(controller, StratResources::Food) + 
        LowResources(controller, StratResources::Energy) + 
        LowResources(controller, StratResources::Production);

    return lowResources;
}

void FA_GatherResources::PerformAction(Faction_AIController& controller)
{
    Faction* faction = controller.GetFaction();

    int lowestResource = 0;
    int highestScore = 0;

    for (int i = 1; i < 4; i++)
    {
        int score = LowResources(controller, StratResources(i));
        if (score > highestScore)
        {
            lowestResource = i;
            highestScore = score;
        }
    }

    TArray<ABaseHex*> hexes = faction->GetHexesOfResource(StratResources(lowestResource));

    ABaseHex* selected = hexes[FMath::RandRange(0, hexes.Num() - 1)];

    UnitActions::AddWorkers(faction->GetFaction(), WorkerType::Alien, selected->GetMaxWorkers(), selected);
}

int FA_GatherResources::LowResources(Faction_AIController& controller, StratResources resource)
{
    Faction* faction = controller.GetFaction();

    int currResource = faction->resourceInventory[resource].currentResources;
    int maxResource = faction->resourceInventory[resource].maxResources;
    int lowResource = FMath::RoundToInt(maxResource * lowResourcePercent);

    if (currResource <= lowResource)
    {
        return 10 * (1.f - ((float)currResource / (float)maxResource));
    }

    return 0;
}
