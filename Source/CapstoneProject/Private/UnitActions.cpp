// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.h"
#include "BaseHex.h"
#include "MergedArmy.h"
#include "Troop.h"
#include "Settler.h"
#include "BattleObject.h"
#include "Building.h"
#include "Outpost.h"
#include "UnitStats.h"
#include "BuildingAttachment.h"
#include "GlobalSpawner.h"
#include "Faction.h"
#include "stdlib.h"


bool UnitActions::IsHostileTarget(AMovementAI* unit, AMovementAI* target)
{
    if (unit->GetUnitData() && target->GetUnitData())
    {
        Factions a = unit->GetUnitData()->GetFaction();
        Factions b = target->GetUnitData()->GetFaction();

        Faction* factionObject = GetFaction(a);
        if (!factionObject) return false;

        if (factionObject->GetFactionRelationship(b) == FactionRelationship::Enemy)
        {
            return true;
        }
    }

    return false;
}

bool UnitActions::IsHostileTarget(const Factions& a, const Factions& b)
{
    Faction* aFaction = GetFaction(a);
    if (!aFaction) return false;

    return aFaction->GetFactionRelationship(b) == FactionRelationship::Enemy;
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

    Faction* factionObject = GetFaction(unitFaction);

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
                FactionRelationship factionAlignment = factionObject->GetFactionRelationship(group1[i]);

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
            FactionRelationship factionAlignment = factionObject->GetFactionRelationship(group1[i]);

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
            FactionRelationship factionAlignment = factionObject->GetFactionRelationship(group1[i]);

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

int UnitActions::GetAvailableWorkerType(Factions faction, WorkerType worker)
{
    return ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
}

int UnitActions::AddWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex)
{
    int availableWorkers = ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
    int workersInHex = 0;
    for (auto& workers : hex->workersInHex)
    {
        workersInHex += workers.Value;
    }
    if (workersInHex >= hex->GetMaxWorkers()) return 0;
    int workersToAdd = availableWorkers >= desiredWorkers ? desiredWorkers : availableWorkers;

    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available -= workersToAdd;
    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].working += workersToAdd;

    return workersToAdd;
}

int UnitActions::AddWorkers(Factions faction, WorkerType worker, int desiredWorkers, AOutpost* outpost, BuildingAttachments attachment)
{
    int availableWorkers = ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
    int workersInHex = 0;

    UBuildingAttachment* selectedAttachment = outpost->GetAttachment(attachment);

    for (auto& workers : selectedAttachment->workersInAttachment)
    {
        workersInHex += workers.Value;
    }
    if (workersInHex >= selectedAttachment->GetMaxWorkers()) return 0;
    int workersToAdd = availableWorkers >= desiredWorkers ? desiredWorkers : availableWorkers;

    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available -= workersToAdd;
    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].working += workersToAdd;

    return workersToAdd;
}

int UnitActions::RemoveWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex)
{
    int workersToRemove = desiredWorkers <= hex->workersInHex[worker] ? desiredWorkers : hex->workersInHex[worker];

    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available += workersToRemove;
    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].working -= workersToRemove;

    return workersToRemove;
}

int UnitActions::RemoveWorkers(Factions faction, WorkerType worker, int desiredWorkers, AOutpost* outpost, BuildingAttachments attachment)
{
    int workersToRemove = desiredWorkers <= outpost->GetAttachment(attachment)->workersInAttachment[worker] ? desiredWorkers : outpost->GetAttachment(attachment)->workersInAttachment[worker];

    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available += workersToRemove;
    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].working -= workersToRemove;

    return workersToRemove;
}

bool UnitActions::SetWorkers(Factions faction, WorkerType worker, int desiredWorkers)
{
    if (desiredWorkers < 0 && FMath::Abs(desiredWorkers) > ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available) return false;

    ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available += desiredWorkers;

    return true;
}

int UnitActions::SetWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex)
{
    if (desiredWorkers > hex->workersInHex[worker])
    {
        return AddWorkers(faction, worker, desiredWorkers - hex->workersInHex[worker], hex);
    }
    else if (desiredWorkers < hex->workersInHex[worker])
    {
        return -RemoveWorkers(faction, worker, hex->workersInHex[worker] - desiredWorkers, hex);
    }
    
    return 0;
}

int UnitActions::SetWorkers(Factions faction, WorkerType worker, int desiredWorkers, AOutpost* outpost, BuildingAttachments attachment)
{
    UBuildingAttachment* attachmentType = outpost->GetAttachment(attachment);

    if (desiredWorkers > attachmentType->workersInAttachment[worker])
    {
        return AddWorkers(faction, worker, desiredWorkers - attachmentType->workersInAttachment[worker], outpost, attachment);
    }
    else if (desiredWorkers < attachmentType->workersInAttachment[worker])
    {
        return -RemoveWorkers(faction, worker, attachmentType->workersInAttachment[worker] - desiredWorkers, outpost, attachment);
    }

    return 0;
}

TArray<int> UnitActions::GetFactionResources(Factions faction)
{
    TArray<int> resources;
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resources.Add(resource.Value.currentResources);
    }
    return resources;
}

void UnitActions::SetFactionResources(Factions faction, StratResources resourceToChange, int desiredResourceVal)
{
    ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resourceToChange].currentResources = desiredResourceVal;
}

int UnitActions::GetFactionPopulation(Factions faction)
{
    int humanPop = ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[WorkerType::Human].available + ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[WorkerType::Human].working;
    return humanPop;
}

TMap<StratResources, int> UnitActions::GetMoreSpecificFactionResources(Factions faction)
{
    TMap<StratResources, int> resources;
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resources.Add(resource.Key, resource.Value.currentResources);
    }
    return resources;
}

TMap<StratResources, int> UnitActions::GetResourcesPerTick(Factions faction)
{
    TMap<StratResources, int> resources;

    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resources.Add(resource.Key, resource.Value.resourcePerTick);
    }
    return resources;
}

TMap<StratResources, int> UnitActions::GetResourceGains(Factions faction)
{
    TMap<StratResources, int> resources;

    resources.Add(StratResources::Wealth, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Wealth].resourcePerTick);
    resources.Add(StratResources::Energy, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Energy].resourcePerTick);
    resources.Add(StratResources::Food, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Food].resourcePerTick);
    resources.Add(StratResources::Production, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Production].resourcePerTick);

    return resources;
}

TMap<StratResources, int> UnitActions::GetResourceLosses(Factions faction)
{
    TMap<StratResources, int> resources;

    resources.Add(StratResources::Wealth, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Wealth].lossesPerDay);
    resources.Add(StratResources::Energy, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Energy].lossesPerDay);
    resources.Add(StratResources::Food, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Food].lossesPerDay);
    resources.Add(StratResources::Production, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Production].lossesPerDay);

    return resources;
}

TMap<WorkerType, int> UnitActions::GetFactionWorkers(Factions faction)
{
    TMap<WorkerType, int> workers;
    for (auto& workerType : ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers)
    {
        workers.Add(workerType.Key, workerType.Value.available);
    }

    return workers;
}

TMap<WorkerType, int> UnitActions::GetWorkerEnergyCost(Factions faction)
{
    TMap<WorkerType, int> workers;
    for (auto& workerType : ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers)
    {
        workers.Add(workerType.Key, workerType.Value.workingEnergyCost);
    }

    return workers;
}

void UnitActions::ConsumeSpentResources(Factions faction, TMap<StratResources, int> resources, ABaseHex* hex)
{
    for (auto& resource : resources)
    {
        ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resource.Key].currentResources -= resource.Value;
        ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resource.Key].currentResources = FMath::Max(0, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resource.Key].currentResources);
    }
}

void UnitActions::ConsumeSpentResources(Factions faction, TMap<StratResources, int> resources, TMap<WorkerType, int> workers, AOutpost* outpost, BuildingAttachments attachment)
{
    for (auto& resource : resources)
    {
        ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resource.Key].currentResources -= resource.Value;
    }

    if (outpost)
    {
        UBuildingAttachment* selectedAttachment = outpost->GetAttachment(attachment);
        for (auto& worker : workers)
        {
            selectedAttachment->workersInAttachment[worker.Key] += UnitActions::AddWorkers(faction, worker.Key, worker.Value, outpost, attachment);
        }
    }
}

void UnitActions::ConsumeSpentResources(Factions faction, TArray<int> values)
{
    int index = 0;
    for (auto& resources : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resources.Key].currentResources = FMath::Clamp(values[index], 0, resources.Value.maxResources);
        index++;
    }
    for (auto& workers : ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers)
    {
        ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[workers.Key].available = values[index] > 0 ? values[index] : 0;
        index++;
    }
}

void UnitActions::UpdateResourceCapacity(Factions faction, int addedCap)
{
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resource.Value.maxResources += addedCap;
    }
}

int UnitActions::GetResourceCap(Factions faction)
{
    return ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Food].maxResources;
}

ABaseHex* UnitActions::GetClosestOutpostHex(Factions faction, AActor* referencePoint)
{
    TArray<AOutpost*> outposts;

    for (ABuilding* building : ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings)
    {
        AOutpost* outpost = Cast<AOutpost>(building);
        if (outpost) outposts.Add(outpost);
    }

    float shortestDistance = std::numeric_limits<float>::infinity();
    AOutpost* closestOutpost = nullptr;
    for (int i = 0; i < outposts.Num(); i++)
    {
        float distance = FVector::Distance(outposts[i]->GetActorLocation(), referencePoint->GetActorLocation());
        if (distance < shortestDistance)
        {
            shortestDistance = distance;
            closestOutpost = outposts[i];
        }
    }

    if (closestOutpost)
    {
        ABaseHex* outpostHex = closestOutpost->hexNav->GetCurrentHex();
        if (outpostHex) return outpostHex;
    }

    return Cast<ABaseHex>(referencePoint);
}

bool UnitActions::HexHasFriendlyTroop(Factions faction, AActor* hex, ATroop* referenceTroop)
{
    ABaseHex* hexActor = Cast<ABaseHex>(hex);
    if (!hexActor) return false;

    for (int i = 0; i < hexActor->troopsInHex.Num(); i++)
    {
        if (hexActor->troopsInHex[i]->GetUnitData()->GetFaction() == faction)
        {
            if (referenceTroop)
            {
                if (hexActor->troopsInHex[i] == referenceTroop) continue;
            }

            return true;
        }
    }

    return false;
}

bool UnitActions::HexHasEnemyTroop(Factions faction, AActor* hex)
{
    ABaseHex* hexActor = Cast<ABaseHex>(hex);
    if (!hexActor) return false;
    Faction* factionObject = GetFaction(faction);

    for (int i = 0; i < hexActor->troopsInHex.Num(); i++)
    {
        if (factionObject->GetFactionRelationship(hexActor->troopsInHex[i]->GetUnitData()->GetFaction()) == FactionRelationship::Enemy)
        {
            return true;
        }
    }

    return false;
}

int UnitActions::GetFactionStarveLevel(Factions faction)
{
    int level = 0;
    if (ACapstoneProjectGameModeBase::activeFactions[faction]->starving)
    {
        ++level;
        if (ACapstoneProjectGameModeBase::activeFactions[faction]->currStarveDays >= ACapstoneProjectGameModeBase::activeFactions[faction]->daysTillStarve)
        {
            ++level;
        }
    }
    
    return level;
}

int UnitActions::GetFactionPowerOutageLevel(Factions faction)
{
    int level = 0;
    if (ACapstoneProjectGameModeBase::activeFactions[faction]->powerOutage)
    {
        ++level;
        if (ACapstoneProjectGameModeBase::activeFactions[faction]->currPowerDays >= ACapstoneProjectGameModeBase::activeFactions[faction]->daysTillPowerOutage)
        {
            ++level;
        }
    }

    return level;
}

void UnitActions::EnableRobots(Factions faction, bool enable)
{
    for (ATroop* troop : ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits)
    {
        troop->interact->SetInteract(enable);
        if (!enable) troop->CancelPath();
    }
}

void UnitActions::RobotIsActive(Factions faction, ATroop* robot)
{
    if (faction == Factions::None) return;

    if (GetFaction(faction)->powerOutage)
    {
        robot->interact->SetInteract(false);
    }
}

void UnitActions::AddResources(Factions faction, TMap<StratResources, int> resources)
{
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        if (!resources.Contains(resource.Key)) continue;
        resource.Value.currentResources += resources[resource.Key];
    }
}

//Called every scan tick, adds found hex to TargetList if hex contains hostile target, removes if target of interest is no longer found there
void UnitActions::SetTargetListElement(Factions faction, AActor* target)
{
    //Return if target actor does not exist
    if (!target) return;

    //Get the faction object and return if it doesn't exist or is not AI-controlled
    Faction* factionObject = GetFaction(faction);
    if (!factionObject) return;
    if (!factionObject->IsAIControlled()) return;

    //Create an easier-to-read address to the faction's target list
    TMap<ABaseHex*, Factions>& targetList = factionObject->targetList;

    //Get the object type of the target actor, return if it isn't a hex
    UnitActions::SelectionIdentity objectType = DetermineObjectType(target);
    if (objectType.type != ObjectTypes::Hex) return;

    //ADD HEX TO LIST IF:

    //(a) Hex contains an enemy troop
    Factions hostileInHex = FindHostileTarget(faction, objectType.hex);
    if (hostileInHex != Factions::None)
    {
        targetList.Add(objectType.hex, hostileInHex);
        return;
    }

    //(b) Hex contains a battle the faction connects with diplomatically
    if (objectType.hex->battle && !objectType.hex->battle->IsEnding())
    {
        Factions hostileInBattle = FindHostileTarget(faction, objectType.hex->battle);
        if (hostileInBattle != Factions::None)
        {
            targetList.Add(objectType.hex, hostileInBattle);
            return;
        }
    }

    //(c) Hex contains an enemy building this faction is not currently occupying
    if (ABuilding* building = objectType.hex->building)
    {
        if (GetFaction(faction)->GetFactionRelationship(building) != FactionRelationship::Enemy) return;
        if (building->GetOccupier() != faction && building->GetUnitData()->IsAlive())
        {
            targetList.Add(objectType.hex, building->GetUnitData()->GetFaction());
            return;
        }
    }

    //If this hex was on the target list and the above conditions do not trigger on subsequent ticks, remove from the list
    if (targetList.Contains(objectType.hex))
    {
        targetList.Remove(objectType.hex);
    }
}

void UnitActions::RemoveFromAllTargetLists(ABaseHex* target)
{
    for (auto& faction : ACapstoneProjectGameModeBase::activeFactions)
    {
        if (faction.Value->targetList.Contains(target))
        {
            faction.Value->targetList.Remove(target);
        }
    }
}

TMap<ABaseHex*, Factions> UnitActions::GetTargetList(Factions faction)
{
    TMap<ABaseHex*, Factions> targetList = TMap<ABaseHex*, Factions>();

    if (ACapstoneProjectGameModeBase::activeFactions.Contains(faction))
    {
        targetList = ACapstoneProjectGameModeBase::activeFactions[faction]->targetList;
    }

    return targetList;
}

bool UnitActions::ArmyContainsUnit(AMovementAI* troop, UnitTypes type, int& unitIndex)
{
    if (troop->GetUnitData()->GetUnitType() == type) return true;

    TArray<FUnitData*> units = troop->GetUnitData()->GetSavedUnits();

    for (int i = 0; i < units.Num(); i++)
    {
        if (units[i]->GetUnitType() == type)
        {
            unitIndex = i;
            return true;
        }
    }

    return false;
}

Factions UnitActions::FindHostileTarget(Factions referenceFaction, ABaseHex* hex)
{
    Faction* factionObject = GetFaction(referenceFaction);

    for (int i = 0; i < hex->troopsInHex.Num(); i++)
    {
        Factions targetFaction = hex->troopsInHex[i]->GetUnitData()->GetFaction();

        if (factionObject->GetFactionRelationship(targetFaction) == FactionRelationship::Enemy)
        {
            return targetFaction;
        }
    }

    return Factions::None;
}

Factions UnitActions::FindHostileTarget(Factions referenceFaction, ABattleObject* battle)
{
    Faction* factionObject = GetFaction(referenceFaction);

    if (!battle->currentBattle.Group1.IsEmpty())
    {
        //Check Group 1 relationships to this unit's faction
        for (int i = 0; i < battle->currentBattle.Group1.Num(); i++)
        {
            FactionRelationship factionAlignment = factionObject->GetFactionRelationship(battle->currentBattle.Group1[i]);
            if (factionAlignment == FactionRelationship::Enemy) return battle->currentBattle.Group1[i];
        }
    }

    if (!battle->currentBattle.Group2.IsEmpty())
    {
        //Check Group 2 relationships to this unit's faction
        for (int i = 0; i < battle->currentBattle.Group2.Num(); i++)
        {
            FactionRelationship factionAlignment = factionObject->GetFactionRelationship(battle->currentBattle.Group2[i]);
            if (factionAlignment == FactionRelationship::Enemy) return battle->currentBattle.Group1[i];
        }
    }

    return Factions::None;
}

Faction* UnitActions::GetFaction(const Factions& faction)
{
    if (!ACapstoneProjectGameModeBase::activeFactions.Contains(faction)) return nullptr;

    return ACapstoneProjectGameModeBase::activeFactions[faction];
}

TMap<Factions, Faction*> UnitActions::GetFactions()
{
    return ACapstoneProjectGameModeBase::activeFactions;
}

UnitActions::SelectionIdentity UnitActions::DetermineObjectType(AActor* object)
{
    SelectionIdentity Results{nullptr, nullptr, nullptr, nullptr, nullptr, ObjectTypes::NoType };

    Results.actor = object;

    ABaseHex* testForHex = Cast<ABaseHex>(object);
    if (testForHex)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("BaseHex found!"));

        Results.type = ObjectTypes::Hex;
        Results.hex = testForHex;
        return Results;
    }

    ATroop* testForAI = Cast<ATroop>(object);
    if (testForAI)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("MovementAI found!"));

        Results.type = ObjectTypes::MoveAI;
        Results.moveAI = testForAI;
        return Results;
    }

    ABuilding* testForBuilding = Cast<ABuilding>(object);
    if (testForBuilding)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Building found!"));

        Results.building = testForBuilding;
        Results.type = ObjectTypes::Building;
        return Results;
    }

    ABattleObject* testForBattle = Cast<ABattleObject>(object);
    if (testForBattle)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Battle found!"));

        Results.battle = testForBattle;
        Results.type = ObjectTypes::Battle;
        return Results;
    }

    return Results;
}