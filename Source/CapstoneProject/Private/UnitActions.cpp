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


bool UnitActions::IsHostileTarget(AMovementAI* unit, AMovementAI* target)
{
    if (unit->unitStats && target->unitStats)
    {
        Faction* factionObject = GetFaction(unit->unitStats->faction);
        if (!factionObject) return false;

        if (factionObject->GetFactionRelationship(target->unitStats->faction) == FactionRelationship::Enemy)
        {
            return true;
        }
    }

    return false;
}

bool UnitActions::IsHostileTarget(AMovementAI* unit, AActor* target)
{
    if (unit->unitStats)
    {
        Faction* factionObject = GetFaction(unit->unitStats->faction);
        if (!factionObject) return false;

        if (factionObject->GetFactionRelationship(target) == FactionRelationship::Enemy)
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

    int speed = 0;
    int bestVision = 0;
    for (int i = 0; i < unit->savedUnits.Num(); i++)
    {
        speed += unit->savedUnits[i].speed;

        if (unit->savedUnits[i].vision > bestVision)
            bestVision = unit->savedUnits[i].vision;
    }
    speed = FMath::RoundToInt((float)speed / (float)unit->savedUnits.Num());

    unit->speed = speed;
    unit->vision = bestVision;

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

    int speed = 0;
    int bestVision = 0;
    for (int i = 0; i < unit.savedUnits.Num(); i++)
    {
        speed += unit.savedUnits[i].speed;

        if (unit.savedUnits[i].vision > bestVision)
            bestVision = unit.savedUnits[i].vision;
    }
    speed = FMath::RoundToInt((float)speed / (float)unit.savedUnits.Num());

    unit.speed = speed;
    unit.vision = bestVision;

    unit.damage += data.damage;
    unit.siegePower += data.siegePower;

    unit.reinforceRate += data.reinforceRate;
    unit.energyUpkeep += data.energyUpkeep;

    return unit;
}

UnitActions::UnitData UnitActions::ExtractUnit(UUnitStats* army, int32 unitIndex)
{
    UnitData unit;

    if (!army->savedUnits.IsValidIndex(unitIndex)) return unit;

    unit = army->savedUnits[unitIndex];
    army->savedUnits.RemoveAt(unitIndex);

    float hpPercent = (float)army->currentHP / (float)army->maxHP;
    float moralePercent = (float)army->currentMorale / (float)army->maxMorale;

    army->maxHP -= unit.maxHP;
    army->currentHP = FMath::RoundToInt((float)army->maxHP * hpPercent);
    army->maxMorale -= unit.maxMorale;
    army->currentMorale = FMath::RoundToInt((float)army->maxMorale * moralePercent);

    int speed = 0;
    int bestVision = 0;
    for (int i = 0; i < army->savedUnits.Num(); i++)
    {
        speed += army->savedUnits[i].speed;

        if (army->savedUnits[i].vision > bestVision)
            bestVision = army->savedUnits[i].vision;
    }
    speed = FMath::RoundToInt((float)speed / (float)army->savedUnits.Num());

    army->damage -= unit.damage;
    army->siegePower -= unit.siegePower;

    army->reinforceRate -= unit.reinforceRate;
    army->energyUpkeep -= unit.energyUpkeep;

    unit.currentHP = FMath::RoundToInt((float)unit.maxHP * hpPercent);
    unit.currentMorale = FMath::RoundToInt((float)unit.maxMorale * hpPercent);

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

bool UnitActions::HexHasFriendlyTroop(Factions faction, AActor* hex, ATroop* referenceTroop)
{
    ABaseHex* hexActor = Cast<ABaseHex>(hex);
    if (!hexActor) return false;

    for (int i = 0; i < hexActor->troopsInHex.Num(); i++)
    {
        if (hexActor->troopsInHex[i]->unitStats->faction == faction)
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
        if (factionObject->GetFactionRelationship(hexActor->troopsInHex[i]->unitStats->faction) == FactionRelationship::Enemy)
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
    if (objectType.hex->battle)
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
        if (building->GetOccupier() != faction && building->unitStats->currentHP > 0.f)
        {
            targetList.Add(objectType.hex, building->unitStats->faction);
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
    if (troop->unitStats->unitType == type) return true;

    for (int i = 0; i < troop->unitStats->savedUnits.Num(); i++)
    {
        if (troop->unitStats->savedUnits[i].unitType == type)
        {
            unitIndex = i;
            return true;
        }
    }

    return false;
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

void UnitActions::GenerateArmyName(Factions namingFaction, AMovementAI* unit, FString newName)
{
    if (!ACapstoneProjectGameModeBase::activeFactions.Contains(namingFaction)) return;

    TMap<FString, TArray<int32>>& names = unit->unitStats->faction == Factions::Human ?
        ACapstoneProjectGameModeBase::activeFactions[namingFaction]->armyNamesHuman :
        ACapstoneProjectGameModeBase::activeFactions[namingFaction]->armyNamesAlien;

    int32& cap = ACapstoneProjectGameModeBase::activeFactions[namingFaction]->maxNameShare;

    if (!names.Contains(newName)) names.Add(newName, TArray<int32>());

    if (newName != TEXT(""))
    {
        if (names[newName].Num() < cap)
        {
            int32 num = 0;
            for (int i = 1; i < cap; i++)
            {
                if (!names[newName].Contains(i))
                {
                    num = i;
                    names[newName].Add(i);
                    break;
                }
            }

            unit->unitStats->name = newName;
            unit->unitStats->nameInstance = num;
            if (num != 0) return;
        }
    }
    
    for (auto& name : names)
    {
        if (name.Value.Num() < cap)
        {
            int32 num = 0;
            for (int i = 1; i < cap; i++)
            {
                if (!name.Value.Contains(i))
                {
                    num = i;
                    name.Value.Add(i);
                    break;
                }
            }

            unit->unitStats->name = name.Key;
            unit->unitStats->nameInstance = num;
            
            if (num != 0) break;
        }
    }
}

void UnitActions::RemoveArmyName(Factions namingFaction, AMovementAI* unit)
{
    if (!ACapstoneProjectGameModeBase::activeFactions.Contains(namingFaction)) return;

    TMap<FString, TArray<int32>>& names = unit->unitStats->faction == Factions::Human ?
        ACapstoneProjectGameModeBase::activeFactions[namingFaction]->armyNamesHuman :
        ACapstoneProjectGameModeBase::activeFactions[namingFaction]->armyNamesAlien;

    if (names.Contains(unit->unitStats->name) && names[unit->unitStats->name].Contains(unit->unitStats->nameInstance))
    {
        names[unit->unitStats->name].Remove(unit->unitStats->nameInstance);
    }
}

Factions UnitActions::FindHostileTarget(Factions referenceFaction, ABaseHex* hex)
{
    Faction* factionObject = GetFaction(referenceFaction);

    for (int i = 0; i < hex->troopsInHex.Num(); i++)
    {
        if (factionObject->GetFactionRelationship(hex->troopsInHex[i]->unitStats->faction) == FactionRelationship::Enemy)
        {
            return hex->troopsInHex[i]->unitStats->faction;
        }
    }

    return Factions::None;
}

Factions UnitActions::FindHostileTarget(Factions referenceFaction, ABattleObject* battle)
{
    Faction* factionObject = GetFaction(referenceFaction);

    //Check Group 1 relationships to this unit's faction
    for (int i = 0; i < battle->currentBattle.Group1.Num(); ++i)
    {
        FactionRelationship factionAlignment = factionObject->GetFactionRelationship(battle->currentBattle.Group1[i]);
        if (factionAlignment == FactionRelationship::Enemy) return battle->currentBattle.Group1[i];
    }

    //Check Group 2 relationships to this unit's faction
    for (int i = 0; i < battle->currentBattle.Group2.Num(); ++i)
    {
        FactionRelationship factionAlignment = factionObject->GetFactionRelationship(battle->currentBattle.Group2[i]);
        if (factionAlignment == FactionRelationship::Enemy) return battle->currentBattle.Group1[i];
    }

    return Factions::None;
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
