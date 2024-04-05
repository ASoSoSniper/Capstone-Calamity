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
    //If comparing unit faction to itself, return Ally.
    if (unitFaction == targetFaction) return FactionRelationship::Ally;

    Faction* faction;

    //If unit faction doesn't exist, return Ally.
    if (ACapstoneProjectGameModeBase::activeFactions.Contains(unitFaction))
    {
        faction = ACapstoneProjectGameModeBase::activeFactions[unitFaction];
    }
    else
    {
        return FactionRelationship::Ally;
    }

    //If unit faction has a relationship with target faction, return that relationship.
    if (faction->factionRelationships.Contains(targetFaction))
    {
        return faction->factionRelationships[targetFaction];
    }

    //Otherwise, return Neutral.
    return FactionRelationship::Neutral;
}

UnitActions::UnitData UnitActions::CollectUnitData(UUnitStats* unit)
{
    UnitData data{
        unit->faction,
        unit->unitType, 
        unit->upgraded,

        unit->currentHP, 
        unit->maxHP, 
        unit->currentMorale,
        unit->maxMorale,

        unit->vision, 
        unit->speed, 
        
        unit->damage,
        unit->siegePower,

        unit->reinforceRate,
        unit->energyUpkeep};
    
    if (data.unitType == UnitTypes::Army)
    {
        for (int i = 0; i < unit->savedUnits.Num(); ++i)
        {
            data.savedUnits.Add(unit->savedUnits[i]);
        }
    }
    
    return data;
}

void UnitActions::ApplyDataToUnitStats(UUnitStats* unit, UnitData& data)
{
    unit->faction = data.faction;
    unit->unitType = data.unitType;
    unit->upgraded = data.upgraded;

    unit->currentHP = data.currentHP;
    unit->maxHP = data.maxHP;
    unit->currentMorale = data.currentMorale;
    unit->maxMorale = data.maxMorale;

    unit->vision = data.vision;
    unit->speed = data.speed;

    unit->damage = data.damage;
    unit->siegePower = data.siegePower;

    unit->reinforceRate = data.reinforceRate;
    unit->energyUpkeep = data.energyUpkeep;
}

void UnitActions::ApplyDataToUnitStats(UUnitStats* unit, FTroopStats& data)
{
    unit->unitType = data.type;

    unit->currentHP = data.HP;
    unit->maxHP = data.HP;
    unit->currentMorale = data.morale;
    unit->maxMorale = data.morale;

    unit->vision = data.vision;
    unit->speed = data.speed;

    unit->damage = data.damage;
    unit->siegePower = data.siegePower;

    unit->reinforceRate = data.reinforceRate;
    unit->energyUpkeep = data.energyUpkeepCost;
}

void UnitActions::AddUnitData(UUnitStats* unit, UnitData& data)
{
    unit->currentHP += data.currentHP;
    unit->maxHP += data.maxHP;
    unit->currentMorale += data.currentMorale;
    unit->maxMorale += data.maxMorale;

    unit->vision += data.vision;
    //unit->speed += data.speed;
    int speed = 0;
    for (int i = 0; i < unit->savedUnits.Num(); i++)
    {
        speed += unit->savedUnits[i].speed;
    }
    speed = FMath::RoundToInt((float)speed / (float)unit->savedUnits.Num());
    unit->speed = speed;

    unit->damage += data.damage;
    unit->siegePower += data.siegePower;

    unit->reinforceRate += data.reinforceRate;
    unit->energyUpkeep += data.energyUpkeep;
}

UnitActions::UnitData UnitActions::AddUnitData(UnitData& unit, UnitData& data)
{
    unit.currentHP += data.currentHP;
    unit.maxHP += data.maxHP;
    unit.currentMorale += data.currentMorale;
    unit.maxMorale += data.maxMorale;

    unit.vision += data.vision;
    int speed = 0;
    for (int i = 0; i < unit.savedUnits.Num(); i++)
    {
        speed += unit.savedUnits[i].speed;
    }
    speed = FMath::RoundToInt((float)speed / (float)unit.savedUnits.Num());
    unit.speed = speed;

    unit.damage += data.damage;
    unit.siegePower += data.siegePower;

    unit.reinforceRate += data.reinforceRate;
    unit.energyUpkeep += data.energyUpkeep;

    return unit;
}

int UnitActions::GetAvailableWorkerType(Factions faction, WorkerType worker)
{
    return ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
}

int UnitActions::AddWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex)
{
    int availableWorkers = ACapstoneProjectGameModeBase::activeFactions[faction]->availableWorkers[worker].available;
    int workersInHex = 0;
    for (auto workers : hex->workersInHex)
    {
        workersInHex += workers.Value;
    }
    if (workersInHex >= hex->maxWorkers) return 0;
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

    for (auto workers : selectedAttachment->workersInAttachment)
    {
        workersInHex += workers.Value;
    }
    if (workersInHex >= selectedAttachment->maxWorkers) return 0;
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
        return AddWorkers(faction, worker, desiredWorkers - attachmentType->workersInAttachment[worker], outpost, attachmentType->type);
    }
    else if (desiredWorkers < attachmentType->workersInAttachment[worker])
    {
        return -RemoveWorkers(faction, worker, attachmentType->workersInAttachment[worker] - desiredWorkers, outpost, attachmentType->type);
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

TArray<TerrainType> UnitActions::GetNonBuildableTerrain()
{
    return ACapstoneProjectGameModeBase::nonBuildableTerrains;
}

bool UnitActions::HexIsTraversable(AActor* hex)
{
    ABaseHex* hexActor = Cast<ABaseHex>(hex);
    if (!hexActor) return false;

    for (int i = 0; i < ACapstoneProjectGameModeBase::nonBuildableTerrains.Num(); i++)
    {
        if (hexActor->hexTerrain == ACapstoneProjectGameModeBase::nonBuildableTerrains[i]) return false;
    }

    return true;
}

int UnitActions::GetResourceCap(Factions faction)
{
    return ACapstoneProjectGameModeBase::activeFactions[faction]->resourceInventory[StratResources::Food].maxResources;
}

ABaseHex* UnitActions::GetClosestOutpostHex(Factions faction, AActor* referencePoint)
{
    TArray<AOutpost*> outposts;

    for (int i = 0; i < ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings.Num(); i++)
    {
        AOutpost* outpost = Cast<AOutpost>(ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings[i]);
        if (outpost) outposts.Add(outpost);
    }

    float shortestDistance = INFINITY;
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
        ABaseHex* outpostHex = Cast<ABaseHex>(closestOutpost->hexNav->currentHex);
        if (outpostHex) return outpostHex;
    }

    return Cast<ABaseHex>(referencePoint);
}

bool UnitActions::HexHasFriendlyTroop(Factions faction, AActor* hex)
{
    ABaseHex* hexActor = Cast<ABaseHex>(hex);
    if (!hexActor) return false;

    for (int i = 0; i < hexActor->troopsInHex.Num(); i++)
    {
        if (hexActor->troopsInHex[i]->unitStats->faction == faction)
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
    for (int i = 0; i < ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits.Num(); i++)
    {
        ATroop* troop = ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits[i];
        if (troop)
        {
            troop->interact->SetInteract(enable);
            if (!enable) troop->CancelPath();
        }
    }
}

void UnitActions::RobotIsActive(Factions faction, ATroop* robot)
{
    if (faction == Factions::None) return;

    if (ACapstoneProjectGameModeBase::activeFactions[faction]->powerOutage)
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

TMap<UnitTypes, FUnitComposition> UnitActions::GetArmyComposition(ATroop* army)
{
    TMap<UnitTypes, FUnitComposition> units;
    if (!army) return units;

    units.Add(UnitTypes::Infantry, FUnitComposition{});
    units.Add(UnitTypes::Cavalry, FUnitComposition{});
    units.Add(UnitTypes::Scout, FUnitComposition{});
    units.Add(UnitTypes::Ranged, FUnitComposition{});
    units.Add(UnitTypes::Shielder, FUnitComposition{});
    units.Add(UnitTypes::Settler, FUnitComposition{});

    int totalUnits = 0;

    for (int i = 0; i < army->unitStats->savedUnits.Num(); i++)
    {
        ++units[army->unitStats->savedUnits[i].unitType].quantity;
        ++totalUnits;
    }

    for (auto& unit : units)
    {
        unit.Value.compPercent = (float)unit.Value.quantity / (float)totalUnits;
    }

    return units;
}

UnitTypes UnitActions::GetLargestUnitQuantity(ATroop* army)
{
    TMap<UnitTypes, FUnitComposition> composition = GetArmyComposition(army);

    UnitTypes highestType = UnitTypes::None;
    int best = 0;

    for (auto& unit : composition)
    {
        if (unit.Value.quantity > best)
        {
            best = unit.Value.quantity;
            highestType = unit.Key;
        }
    }

    return highestType;
}

void UnitActions::SetTargetListElement(Factions faction, AActor* target, bool addToList)
{
    if (!ACapstoneProjectGameModeBase::activeFactions.Contains(faction) || !target) return;

    UnitActions::SelectionIdentity objectType = DetermineObjectType(target);

    if (!ACapstoneProjectGameModeBase::activeFactions[faction]->targetList.Contains(target))
    {
        if (addToList)
        {
            if (objectType.type == ObjectTypes::Hex) return;

            if (objectType.type != ObjectTypes::Battle)
            {
                if (GetFactionRelationship(faction, target->FindComponentByClass<UUnitStats>()->faction) != FactionRelationship::Enemy) return;
            }

            ACapstoneProjectGameModeBase::activeFactions[faction]->targetList.Add(target);
        }
    }
    else
    {
        if (!addToList)
        {
            if (objectType.type == ObjectTypes::Building || objectType.type == ObjectTypes::Battle) return;

            ACapstoneProjectGameModeBase::activeFactions[faction]->targetList.Remove(target);
        }
    }
}

void UnitActions::RemoveFromAllTargetLists(AActor* target)
{
    for (auto& faction : ACapstoneProjectGameModeBase::activeFactions)
    {
        if (faction.Value->targetList.Contains(target))
        {
            faction.Value->targetList.Remove(target);
        }
    }
}

TArray<AActor*> UnitActions::GetTargetList(Factions faction)
{
    TArray<AActor*> targetList = TArray<AActor*>();

    if (ACapstoneProjectGameModeBase::activeFactions.Contains(faction))
    {
        targetList = ACapstoneProjectGameModeBase::activeFactions[faction]->targetList;
    }

    return targetList;
}

bool UnitActions::CommandTroopToMerge(ATroop* troop, AActor* target)
{
    UHexNav* hexNav = target->GetComponentByClass<UHexNav>();
    UUnitStats* targetStats = target->GetComponentByClass<UUnitStats>();

    if (troop->unitStats->savedUnits.Num() + targetStats->savedUnits.Num() > troop->armyCap) return false;

    troop->hexNav->targetHex = hexNav->currentHex;

    if (troop->hexNav->targetHex)
    {
        troop->merging = true;
        troop->targetToMerge = target;
        troop->CreatePath();

        return true;
    }
    
    return false;
}

void UnitActions::AssignFaction(Factions faction, AActor* target)
{   
    if (ACapstoneProjectGameModeBase::activeFactions.Contains(faction))
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

void UnitActions::AssignFaction(Factions faction, ABaseHex* hex)
{
    if (hex->hexOwner != Factions::None)
    {
        if (ACapstoneProjectGameModeBase::activeFactions[hex->hexOwner]->ownedHexes.Contains(hex))
            ACapstoneProjectGameModeBase::activeFactions[hex->hexOwner]->ownedHexes.Remove(hex);
    }

    hex->hexOwner = faction;
    
    if (faction == Factions::None)
    {
        hex->visibility->hexBaseMaterials.visibleTexture = ACapstoneProjectGameModeBase::factionColors[Factions::None];
        return;
    }

    hex->visibility->hexBaseMaterials.visibleTexture = ACapstoneProjectGameModeBase::activeFactions[faction]->factionColor;

    if (!ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes.Contains(hex))
    {
        ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes.Add(hex);
    }
        
}

void UnitActions::RemoveFromFaction(Factions faction, AActor* target)
{
    if (ACapstoneProjectGameModeBase::activeFactions.Contains(faction))
    {
        ATroop* testForAI = Cast<ATroop>(target);
        if (testForAI)
        {
            if (ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits.Contains(testForAI))
            {
                ACapstoneProjectGameModeBase::activeFactions[faction]->allUnits.Remove(testForAI);
            }

            return;
        }

        ABuilding* testForBuilding = Cast<ABuilding>(target);
        if (testForBuilding)
        {
            if (ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings.Contains(testForBuilding))
            {
                ACapstoneProjectGameModeBase::activeFactions[faction]->allBuildings.Remove(testForBuilding);
            }

            return;
        }

        ABaseHex* testForHex = Cast<ABaseHex>(target);
        if (testForHex)
        {
            if (ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes.Contains(testForHex))
            {
                ACapstoneProjectGameModeBase::activeFactions[faction]->ownedHexes.Remove(testForHex);
            }

            return;
        }
    }
}

Faction* UnitActions::GetFaction(Factions faction)
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
