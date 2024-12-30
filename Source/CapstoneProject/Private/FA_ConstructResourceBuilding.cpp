// Fill out your copyright notice in the Description page of Project Settings.


#include "FA_ConstructResourceBuilding.h"
#include "Outpost.h"

/*
    --Construct Resource Building--

    Score Factors:
        - Available resources match/exceed the cost of a building type (not counting workers)
        - Income/cost ratio of the building's specialized resource is below the desired value
            - Higher score the lower income is relative to cost
    Action:
        - Constructs the selected building on an owned tile
            - Selects tile with a high output of the desired resource
            - Selects tile in close proximity to an outpost
*/

FA_ConstructResourceBuilding::FA_ConstructResourceBuilding()
{
}

FA_ConstructResourceBuilding::~FA_ConstructResourceBuilding()
{
}

int FA_ConstructResourceBuilding::ScoreAction(Faction_AIController& controller)
{
    int score = 0;

    SpawnableBuildings selectedBuilding = GetNeededBuilding(controller);

    if (selectedBuilding != SpawnableBuildings::None) return 0;
    else
        score += 30;

    if (controller.GetFaction()->resourceInventory[StratResources::Production].currentResources >= AGlobalSpawner::spawnerObject->buildingCosts[selectedBuilding].productionCost)
    {
        score += 30;
    }

    return score;
}

void FA_ConstructResourceBuilding::PerformAction(Faction_AIController& controller)
{
    Faction* faction = controller.GetFaction();

    TArray<ABaseHex*> hexes = faction->GetHexesOfResource(StratResources());
    TArray<AOutpost*> outposts = faction->GetFactionOutposts();

    int rand = FMath::RandRange(0, outposts.Num());

    ABaseHex* closestHex = nullptr;
    float closest = INFINITY;
    
    for (int i = 0; i < hexes.Num(); i++)
    {
        float dist = FVector::DistSquaredXY(outposts[rand]->hexNav->GetCurrentHex()->GetActorLocation(), hexes[i]->GetActorLocation());

        if (dist < closest)
        {
            closest = dist;
            closestHex = hexes[i];
        }
    }

    SpawnableBuildings selectedBuilding = GetNeededBuilding(controller);
    AGlobalSpawner::spawnerObject->SpawnBuilding(faction->GetFaction(), selectedBuilding, closestHex);
}

SpawnableBuildings FA_ConstructResourceBuilding::GetNeededBuilding(Faction_AIController& controller)
{
    FResourceGainLoss rates = controller.GetFaction()->GetResourceRates();

    SpawnableBuildings selectedBuilding = SpawnableBuildings::None;

    float food = (float)rates.foodGain / (float)rates.foodLoss;
    float prod = (float)rates.productionGain / (float)rates.productionLoss;
    float energy = (float)rates.energyGain / (float)rates.energyLoss;

    bool lowIncome = false;
    if (food <= lowRatio)
    {
        selectedBuilding = SpawnableBuildings::Farmland;
        lowIncome = true;
    }
    if (prod < food && !lowIncome)
    {
        selectedBuilding = SpawnableBuildings::MiningStation;
        lowIncome = true;
    }
    if (energy < prod && !lowIncome)
    {
        selectedBuilding = SpawnableBuildings::PowerPlant;
        lowIncome = true;
    }

    return selectedBuilding;
}
