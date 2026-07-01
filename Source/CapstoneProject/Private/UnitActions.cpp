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
#include "GlobalSpawner.h"
#include "Faction.h"
#include "FactionController.h"
#include "stdlib.h"


bool UnitActions::IsHostileTarget(AMovementAI* unit, AMovementAI* target)
{
    if (unit->GetUnitData() && target->GetUnitData())
    {
        EFactions a = unit->GetUnitData()->GetFaction();
        EFactions b = target->GetUnitData()->GetFaction();

        UFaction* factionObject = GetFaction(a);
        if (!factionObject) return false;

        if (factionObject->GetFactionRelationship(b) == EFactionRelationship::Enemy)
        {
            return true;
        }
    }

    return false;
}

bool UnitActions::IsHostileTarget(const EFactions& a, const EFactions& b)
{
    UFaction* aFaction = GetFaction(a);
    if (!aFaction) return false;

    return aFaction->GetFactionRelationship(b) == EFactionRelationship::Enemy;
}

bool UnitActions::IsAllyToFaction(EFactionRelationship relationship)
{
    switch (relationship)
    {
    case EFactionRelationship::Ally:
        return true;
    case EFactionRelationship::Enemy:
        return false;
    case EFactionRelationship::Neutral:
        return false;
    }

    return false;
}

int UnitActions::GetAvailableWorkerType(EFactions faction, WorkerType worker)
{
    return ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
}

TArray<int> UnitActions::GetFactionResources(EFactions faction)
{
    TArray<int> resources;
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resources.Add(resource.Value.currentResources);
    }
    return resources;
}

void UnitActions::SetFactionResources(EFactions faction, EStratResources resourceToChange, int desiredResourceVal)
{
    ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[resourceToChange].currentResources = desiredResourceVal;
}

int UnitActions::GetFactionPopulation(EFactions faction)
{
    int humanPop = ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[WorkerType::Human].available + ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[WorkerType::Human].working;
    return humanPop;
}

TMap<EStratResources, int> UnitActions::GetMoreSpecificFactionResources(EFactions faction)
{
    TMap<EStratResources, int> resources;
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resources.Add(resource.Key, resource.Value.currentResources);
    }
    return resources;
}

TMap<EStratResources, int> UnitActions::GetResourcesPerTick(EFactions faction)
{
    TMap<EStratResources, int> resources;

    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resources.Add(resource.Key, resource.Value.resourcePerTick);
    }
    return resources;
}

TMap<EStratResources, int> UnitActions::GetResourceGains(EFactions faction)
{
    TMap<EStratResources, int> resources;

    resources.Add(EStratResources::Wealth, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Wealth].resourcePerTick);
    resources.Add(EStratResources::Energy, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Energy].resourcePerTick);
    resources.Add(EStratResources::Food, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Food].resourcePerTick);
    resources.Add(EStratResources::Production, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Production].resourcePerTick);

    return resources;
}

TMap<EStratResources, int> UnitActions::GetResourceLosses(EFactions faction)
{
    TMap<EStratResources, int> resources;

    resources.Add(EStratResources::Wealth, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Wealth].lossesPerDay);
    resources.Add(EStratResources::Energy, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Energy].lossesPerDay);
    resources.Add(EStratResources::Food, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Food].lossesPerDay);
    resources.Add(EStratResources::Production, ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Production].lossesPerDay);

    return resources;
}

TMap<WorkerType, int> UnitActions::GetFactionWorkers(EFactions faction)
{
    TMap<WorkerType, int> workers;
    for (auto& workerType : ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers)
    {
        workers.Add(workerType.Key, workerType.Value.available);
    }

    return workers;
}

TMap<WorkerType, int> UnitActions::GetWorkerEnergyCost(EFactions faction)
{
    TMap<WorkerType, int> workers;
    for (auto& workerType : ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers)
    {
        workers.Add(workerType.Key, workerType.Value.workingEnergyCost);
    }

    return workers;
}

void UnitActions::UpdateResourceCapacity(EFactions faction, int addedCap)
{
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        resource.Value.maxResources += addedCap;
    }
}

int UnitActions::GetResourceCap(EFactions faction)
{
    return ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[EStratResources::Food].maxResources;
}

ABaseHex* UnitActions::GetClosestOutpostHex(EFactions faction, AActor* referencePoint)
{
    const TSet<ABuilding*>& outposts = ACapstoneProjectGameModeBase::activeFactions[faction]->GetBuildingsOfType(SpawnableBuildings::Outpost);

    float shortestDistance = std::numeric_limits<float>::infinity();
    ABuilding* closestOutpost = nullptr;
    for (ABuilding* outpost : outposts)
    {
        float distance = FVector::Distance(outpost->GetActorLocation(), referencePoint->GetActorLocation());
        if (distance < shortestDistance)
        {
            shortestDistance = distance;
            closestOutpost = outpost;
        }
    }

    if (closestOutpost)
    {
        ABaseHex* outpostHex = closestOutpost->hexNav->GetCurrentHex();
        if (outpostHex) return outpostHex;
    }

    return Cast<ABaseHex>(referencePoint);
}

bool UnitActions::HexHasFriendlyTroop(EFactions faction, AActor* hex, ATroop* referenceTroop)
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

bool UnitActions::HexHasEnemyTroop(EFactions faction, AActor* hex)
{
    ABaseHex* hexActor = Cast<ABaseHex>(hex);
    if (!hexActor) return false;
    UFaction* factionObject = GetFaction(faction);

    for (int i = 0; i < hexActor->troopsInHex.Num(); i++)
    {
        if (factionObject->GetFactionRelationship(hexActor->troopsInHex[i]->GetUnitData()->GetFaction()) == EFactionRelationship::Enemy)
        {
            return true;
        }
    }

    return false;
}

int UnitActions::GetFactionStarveLevel(EFactions faction)
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

int UnitActions::GetFactionPowerOutageLevel(EFactions faction)
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

void UnitActions::EnableRobots(EFactions faction, bool enable)
{
    const TSet<ATroop*>& factionTroops = ACapstoneProjectGameModeBase::activeFactions[faction]->GetTroops();

    for (ATroop* troop : factionTroops)
    {
        troop->interact->SetInteract(enable);
        if (!enable) troop->CancelPath();
    }
}

void UnitActions::RobotIsActive(EFactions faction, ATroop* robot)
{
    if (faction == EFactions::None) return;

    if (GetFaction(faction)->powerOutage)
    {
        robot->interact->SetInteract(false);
    }
}

void UnitActions::AddResources(EFactions faction, TMap<EStratResources, int> resources)
{
    for (auto& resource : ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory)
    {
        if (!resources.Contains(resource.Key)) continue;
        resource.Value.currentResources += resources[resource.Key];
    }
}

//Called every scan tick, adds found hex to TargetList if hex contains hostile target, removes if target of interest is no longer found there
void UnitActions::SetTargetListElement(EFactions faction, AActor* target)
{
    //Return if target actor does not exist
    if (!target) return;

    //Get the faction object and return if it doesn't exist or is not AI-controlled
    UFaction* factionObject = GetFaction(faction);
    if (!factionObject) return;
    if (!factionObject->IsAIControlled()) return;

    //Create an easier-to-read address to the faction's target list
    TMap<ABaseHex*, EFactions>& targetList = factionObject->targetList;

    //Get the object type of the target actor, return if it isn't a hex
    UnitActions::SelectionIdentity objectType = DetermineObjectType(target);
    if (objectType.type != ObjectTypes::Hex) return;

    //ADD HEX TO LIST IF:

    //(a) Hex contains an enemy troop
    EFactions hostileInHex = FindHostileTarget(faction, objectType.hex);
    if (hostileInHex != EFactions::None)
    {
        targetList.Add(objectType.hex, hostileInHex);
        return;
    }

    //(b) Hex contains a battle the faction connects with diplomatically
    if (objectType.hex->battle && !objectType.hex->battle->IsEnding())
    {
        EFactions hostileInBattle = FindHostileTarget(faction, objectType.hex->battle);
        if (hostileInBattle != EFactions::None)
        {
            targetList.Add(objectType.hex, hostileInBattle);
            return;
        }
    }

    //(c) Hex contains an enemy building this faction is not currently occupying
    if (ABuilding* building = objectType.hex->building)
    {
        if (GetFaction(faction)->GetFactionRelationship(building) != EFactionRelationship::Enemy) return;
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

TMap<ABaseHex*, EFactions> UnitActions::GetTargetList(EFactions faction)
{
    TMap<ABaseHex*, EFactions> targetList = TMap<ABaseHex*, EFactions>();

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

EFactions UnitActions::FindHostileTarget(EFactions referenceFaction, ABaseHex* hex)
{
    UFaction* factionObject = GetFaction(referenceFaction);

    for (int i = 0; i < hex->troopsInHex.Num(); i++)
    {
        EFactions targetFaction = hex->troopsInHex[i]->GetUnitData()->GetFaction();

        if (factionObject->GetFactionRelationship(targetFaction) == EFactionRelationship::Enemy)
        {
            return targetFaction;
        }
    }

    return EFactions::None;
}

EFactions UnitActions::FindHostileTarget(EFactions referenceFaction, ABattleObject* battle)
{
    UFaction* factionObject = GetFaction(referenceFaction);

    auto GetHostile = [&](TMap<EFactions, FUnitData*>& group) -> EFactions
        {
            if (group.IsEmpty()) return EFactions::None;

            for (TPair<EFactions, FUnitData*>& army : group)
            {
                EFactionRelationship factionAlignment = factionObject->GetFactionRelationship(army.Key);
                if (factionAlignment == EFactionRelationship::Enemy) return army.Key;
            }

            return EFactions::None;
        };

    EFactions foundHostile = GetHostile(battle->currentBattle.Group1);
    if (foundHostile == EFactions::None)
        foundHostile = GetHostile(battle->currentBattle.Group2);

    return foundHostile;
}

UFaction* UnitActions::GetFaction(const EFactions& faction)
{
    if (!ACapstoneProjectGameModeBase::activeFactions.Contains(faction)) return nullptr;

    return ACapstoneProjectGameModeBase::activeFactions[faction];
}

TMap<EFactions, UFaction*> UnitActions::GetFactions()
{
    return ACapstoneProjectGameModeBase::activeFactions;
}

AFactionController* UnitActions::GetFactionController(const EFactions& faction)
{
    if (UFaction* factionObject = GetFaction(faction))
    {
        return factionObject->GetFactionController();
    }

    return nullptr;
}

UnitActions::SelectionIdentity UnitActions::DetermineObjectType(AActor* object)
{
    SelectionIdentity Results{nullptr, nullptr, nullptr, nullptr, nullptr, ObjectTypes::NoType };

    Results.actor = object;

    ABaseHex* testForHex = Cast<ABaseHex>(object);
    if (testForHex)
    {
        Results.type = ObjectTypes::Hex;
        Results.hex = testForHex;
        return Results;
    }

    ATroop* testForAI = Cast<ATroop>(object);
    if (testForAI)
    {
        Results.type = ObjectTypes::MoveAI;
        Results.moveAI = testForAI;
        return Results;
    }

    ABuilding* testForBuilding = Cast<ABuilding>(object);
    if (testForBuilding)
    {
        Results.building = testForBuilding;
        Results.type = ObjectTypes::Building;
        return Results;
    }

    ABattleObject* testForBattle = Cast<ABattleObject>(object);
    if (testForBattle)
    {
        Results.battle = testForBattle;
        Results.type = ObjectTypes::Battle;
        return Results;
    }

    return Results;
}