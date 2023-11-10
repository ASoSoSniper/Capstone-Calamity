// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.h"
#include "BaseHex.h"
#include "MergedArmy.h"
#include "Troop.h"
#include "BattleObject.h"
#include "Building.h"
#include "UnitStats.h"
#include "Faction.h"


void UnitActions::HarvestResources(Factions faction, int quantity, StratResources resource)
{
    /*if (resource != StratResources::None)
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
    }*/
}

void UnitActions::Attack(TArray<AActor*> attackers, TArray<AActor*> targets)
{
    

    
}

bool UnitActions::IsHostileTarget(AMovementAI* unit, AMovementAI* target)
{
    if (unit->unitStats && target->unitStats)
    {
        if (GetFactionRelationship(unit->unitStats->faction, target->unitStats->faction) == FactionRelationship::Enemy)
        {
            return true;
        }
    }

    return false;
}

bool UnitActions::IsHostileTarget(AMovementAI* unit, AActor* target)
{
    UUnitStats* targetStats = target->FindComponentByClass<UUnitStats>();

    if (unit->unitStats && targetStats)
    {
        if (GetFactionRelationship(unit->unitStats->faction, targetStats->faction) == FactionRelationship::Enemy)
        {
            return true;
        }
    }

    return false;
}

bool UnitActions::IsAllyToFaction(FactionRelationship relationship)
{
    switch (relationship)
    {
    case FactionRelationship::Ally:
        return true;
    case FactionRelationship::Enemy:
        return false;
    case FactionRelationship::Neutral:
        return false;
    }

    return false;
}

EngagementSelect UnitActions::DetermineConflictAlignment(Factions& unitFaction, TArray<Factions>& group1, TArray<Factions>& group2)
{
    //
    //  EMPTY GROUPS:
    //  If one or both groups are empty, perform a more complex relationship check:
    // 
    
    //If Group 1 is empty, Group 2 is also empty, so this unit can join Group 1 to start things off
    if (group1.IsEmpty())
    {
        return EngagementSelect::JoinGroup1;
    }

    //If Group 2 is empty, check whether Group 1 is composed entirely of enemies or allies, and assign accordingly
    if (group2.IsEmpty())
    {
        //Declare variables to record each possible relationship in Group 1
        int alliesInGroup1 = 0;
        int neutralsInGroup1 = 0;
        int enemiesInGroup1 = 0;

        //Check Group 1 relationships to this unit's faction
        for (int i = 0; i < group1.Num(); ++i)
        {
            if (group1[i] != unitFaction)
            {
                FactionRelationship factionAlignment = GetFactionRelationship(unitFaction, group1[i]);

                //Record the faction's relationship
                switch (factionAlignment)
                {
                case FactionRelationship::Ally:
                    alliesInGroup1++;
                    break;
                case FactionRelationship::Neutral:
                    neutralsInGroup1++;
                    break;
                case FactionRelationship::Enemy:
                    enemiesInGroup1++;
                    break;
                }
            }
        }
        //If Group 1 is composed entirely of enemies and Group 2 is empty, this unit joins Group 2
        if (alliesInGroup1 == 0 && neutralsInGroup1 == 0 && enemiesInGroup1 > 0)
        {
            return EngagementSelect::JoinGroup2;
        }
        //If Group 1 is composed entirely of allies, join Group 1
        else if (alliesInGroup1 > 0 && neutralsInGroup1 == 0 && enemiesInGroup1 == 0)
        {
            return EngagementSelect::JoinGroup1;
        }

        //Otherwise, do not join the conflict
        return EngagementSelect::DoNotJoin;
    }    

    // 
    //  NON-EMPTY GROUPS:
    //  If both groups possess factions, perform a more streamlined check:
    //

    bool cannotJoinGroup1 = false;
    bool cannotJoinGroup2 = false;

    //Check Group 1 relationships to this unit's faction
    for (int i = 0; i < group1.Num(); ++i)
    {
        if (group1[i] != unitFaction)
        {
            FactionRelationship factionAlignment = GetFactionRelationship(unitFaction, group1[i]);

            //If this faction not a direct ally, deny Group 1 as a possible selection
            if (!IsAllyToFaction(factionAlignment))
            {
                cannotJoinGroup1 = true;
            }
        }
    }

    //Check Group 2 relationships to this unit's faction
    for (int i = 0; i < group2.Num(); ++i)
    {
        if (group2[i] != unitFaction)
        {
            FactionRelationship factionAlignment = GetFactionRelationship(unitFaction, group2[i]);

            //If this faction not a direct ally, deny Group 2 as a possible selection
            if (!IsAllyToFaction(factionAlignment))
            {
                cannotJoinGroup2 = true;
            }
        }
    }

    if (!cannotJoinGroup1 && cannotJoinGroup2)
    {
        return EngagementSelect::JoinGroup1;
    }
    else if (cannotJoinGroup1 && !cannotJoinGroup2)
    {
        return EngagementSelect::JoinGroup2;
    }

    return EngagementSelect::DoNotJoin;
}

FactionRelationship UnitActions::GetFactionRelationship(Factions unitFaction, Factions targetFaction)
{
    if (unitFaction == targetFaction) return FactionRelationship::Ally;

    Faction* faction;

    if (ACapstoneProjectGameModeBase::activeFactions.Contains(unitFaction))
    {
        faction = ACapstoneProjectGameModeBase::activeFactions[unitFaction];
    }
    else
    {
        return FactionRelationship::Ally;
    }

    if (faction->factionRelationships.Contains(targetFaction))
    {
        return faction->factionRelationships[targetFaction];
    }

    return FactionRelationship::Neutral;
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
        unit->currentMorale,
        unit->maxMorale,
        unit->minDamage, 
        unit->maxDamage,
        unit->reinforceRate};
    
    if (data.unitType == UnitTypes::Army)
    {
        for (int i = 0; i < unit->savedUnits.Num(); ++i)
        {
            data.savedUnits.Add(unit->savedUnits[i]);
        }
    }
    
    return data;
}

int UnitActions::GetAvailableWorkerType(Factions faction, WorkerType worker)
{
    return ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
}

int UnitActions::AddWorkers(Factions faction, WorkerType worker, int& desiredWorkers, int& workersInHex)
{
    int availableWorkers = ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
    if (workersInHex == 10) return 0;
    int workersToAdd = availableWorkers >= desiredWorkers ? desiredWorkers : availableWorkers;

    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available -= workersToAdd;
    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].working += workersToAdd;

    return workersToAdd;
}

int UnitActions::RemoveWorkers(Factions faction, WorkerType worker, int& desiredWorkers, int& workersInHex)
{
    int workersToRemove = workersInHex >= desiredWorkers ? desiredWorkers : workersInHex;

    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available += workersToRemove;
    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].working -= workersToRemove;

    return workersToRemove;
}

TArray<int> UnitActions::GetFactionResources(Factions faction)
{
    TArray<int> resources;
    for (auto resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resources.Add(resource.Value.currentResources);
    }
    return resources;
}

void UnitActions::AssignFaction(Factions faction, AActor* target)
{   
    if (ACapstoneProjectGameModeBase::activeFactions.Find(faction))
    {
        ATroop* testForAI = Cast<ATroop>(target);
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
    SelectionIdentity Results{nullptr, nullptr, nullptr, nullptr, nullptr, ObjectTypes::NoType };

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

    ABattleObject* testForBattle = Cast<ABattleObject>(object);
    if (testForBattle)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Battle found!"));

        Results.battle = testForBattle;
        Results.type = ObjectTypes::Battle;
        return Results;
    }

    return Results;
}
