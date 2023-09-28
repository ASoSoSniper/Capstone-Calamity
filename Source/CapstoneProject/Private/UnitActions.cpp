// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.h"
#include "BaseHex.h"
#include "Troop.h"
#include "MovementAI.h"
#include "Building.h"
#include "UnitStats.h"
#include "Faction.h"


void UnitActions::HarvestResources(Factions faction, int quantity, StratResources resource)
{
    if (resource != StratResources::none)
    {
        if (ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory.Find(resource))
        {
            ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resource] += quantity;
            GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, FString::Printf(TEXT("%d resources gathered!\nThis faction has %d resources!"), quantity, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resource]));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Missing inventory")));
        }
        
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("No resources found"));
    }
}

void UnitActions::Attack(TArray<AActor*> attackers, TArray<AActor*> targets)
{
    

    
}

bool UnitActions::IsHostileTarget(AMovementAI* unit, AActor* target)
{
    UUnitStats* targetStats = target->FindComponentByClass<UUnitStats>();
    if (unit->unitStats && targetStats)
    {
        //Placeholder until more fleshed-out hostility determination is decided
        if (unit->unitStats->faction == targetStats->faction)
        {
            return false;
        }

        return true;
    }

    return false;
}

UnitActions::UnitData UnitActions::CollectUnitData(UUnitStats* unit)
{
    UnitData data{
        unit->faction,
        unit->type, 
        unit->upgraded, 
        unit->HP_current, 
        unit->HP_max, 
        unit->defense, 
        unit->speed, 
        unit->minDamage, 
        unit->maxDamage };

    return data;
}

void UnitActions::AssignFaction(Factions faction, AActor* target)
{   
    if (ACapstoneProjectGameModeBase::activeFactions.Find(faction))
    {
        AMovementAI* testForAI = Cast<AMovementAI>(target);
        if (testForAI)
        {
            testForAI->unitStats->faction = faction;
            ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits.Add(testForAI);

            return;
        }

        ABuilding* testForBuilding = Cast<ABuilding>(target);
        if (testForBuilding)
        {
            testForBuilding->unitStats->faction = faction;
            ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings.Add(testForBuilding);
            
            return;
        }
    }
}

UnitActions::SelectionIdentity UnitActions::DetermineObjectType(AActor* object)
{
    SelectionIdentity Results{nullptr, nullptr, nullptr, nullptr, ObjectTypes::NoType };

    Results.actor = object;

    ABaseHex* testForHex = Cast<ABaseHex>(object);
    if (testForHex)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("BaseHex found!"));

        Results.type = ObjectTypes::Hex;
        Results.hex = testForHex;
        return Results;
    }

    ATroop* testForAI = Cast<ATroop>(object);
    if (testForAI)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("MovementAI found!"));

        Results.type = ObjectTypes::MoveAI;
        Results.moveAI = testForAI;
        return Results;
    }

    ABuilding* testForBuilding = Cast<ABuilding>(object);
    if (testForBuilding)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Building found!"));

        Results.building = testForBuilding;
        Results.type = ObjectTypes::Building;
        return Results;
    }
    return Results;
}
