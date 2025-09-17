// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FactionEnum.h"
#include "StratResources.h"
#include "TerrainEnum.h"
#include "CoreMinimal.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class GameStates : uint8
{
	None,
	Victory,
	Defeat
};

UENUM()
enum class ObjectTypes
{
	NoType,
	Hex,
	MoveAI,
	Building,
	Battle
};
UENUM(BlueprintType)
enum class ActionStates : uint8
{
	None,
	HexManage,
	BaseManage,
	TroopManage,
	BattleManage
};
UENUM(BlueprintType)
enum class SpawnableBuildings : uint8
{
	None,
	MiningStation,
	Farmland,
	PowerPlant,
	Storage,
	DefenseStation,
	RobotFactory,
	RobotBarracks,
	TradeOutpost,
	Embassy,
	PoliceStation,
	Outpost,
	Capitol,
	AlienCity,
	RockCity
};

UENUM(BlueprintType)
enum class UnitTypes : uint8
{
	None,
	Cavalry,
	Infantry,
	Ranged,
	Shielder,
	Scout,
	Settler,
	Army
};

UENUM()
enum class WorkerType
{
	Human,
	Robot,
	Alien
};

UENUM()
enum class EngagementSelect
{
	DoNotJoin,
	JoinGroup1,
	JoinGroup2
};

class Faction;
class ABaseHex;
class ATroop;
class AMergedArmy;
class ASettler;
class AMovementAI;
class ABuilding;
class UUnitStats;
class ABattleObject;
class AOutpost;
class AGlobalSpawner;
struct FTroopStats;
struct FUnitComposition;

class CAPSTONEPROJECT_API UnitActions
{
public:

	static bool IsHostileTarget(AMovementAI* unit, AMovementAI* target);
	static bool IsHostileTarget(const Factions& a, const Factions& b);

	static bool IsAllyToFaction(FactionRelationship relationship);

	static EngagementSelect DetermineConflictAlignment(Factions& unitFaction, TArray<Factions>& group1, TArray<Factions>& group2);

	static Faction* GetFaction(const Factions& faction);
	static TMap<Factions, Faction*> GetFactions();
	
	struct SelectionIdentity
	{
		AActor* actor;
		ABaseHex* hex;
		ATroop* moveAI;
		ABuilding* building;
		ABattleObject* battle;
		ObjectTypes type;
	};

	static SelectionIdentity DetermineObjectType(AActor* object);

	static int GetAvailableWorkerType(Factions faction, WorkerType worker);

	static int AddWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex);
	static int RemoveWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex);
	static bool SetWorkers(Factions faction, WorkerType worker, int desiredWorkers);
	static int SetWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex);

	static TArray<int> GetFactionResources(Factions faction);
	static void SetFactionResources(Factions faction, StratResources resourceToChange, int desiredResourceVal);
	static int GetFactionPopulation(Factions faction);
	static TMap<StratResources, int> GetMoreSpecificFactionResources(Factions faction);
	static TMap<StratResources, int> GetResourcesPerTick(Factions faction);
	static TMap<StratResources, int> GetResourceGains(Factions faction);
	static TMap<StratResources, int> GetResourceLosses(Factions faction);
	static TMap<WorkerType, int> GetFactionWorkers(Factions faction);
	static TMap<WorkerType, int> GetWorkerEnergyCost(Factions faction);
	static void ConsumeSpentResources(Factions faction, TMap<StratResources, int> resources, ABaseHex* hex = nullptr);
	static void ConsumeSpentResources(Factions faction, TArray<int> values);
	static void UpdateResourceCapacity(Factions faction, int addedCap);

	static int GetResourceCap(Factions faction);
	static ABaseHex* GetClosestOutpostHex(Factions faction, AActor* referencePoint);
	static bool HexHasFriendlyTroop(Factions faction, AActor* hex, ATroop* referenceTroop = nullptr);
	static bool HexHasEnemyTroop(Factions faction, AActor* hex);
	static int GetFactionStarveLevel(Factions faction);
	static int GetFactionPowerOutageLevel(Factions faction);

	static void EnableRobots(Factions faction, bool enable);
	static void RobotIsActive(Factions faction, ATroop* robot);
	static void AddResources(Factions faction, TMap<StratResources, int> resources);

	static void SetTargetListElement(Factions faction, AActor* target);
	static void RemoveFromAllTargetLists(ABaseHex* target);
	static TMap<ABaseHex*, Factions> GetTargetList(Factions faction);

	static bool ArmyContainsUnit(AMovementAI* troop, UnitTypes type, int& unitIndex);

	static Factions FindHostileTarget(Factions referenceFaction, ABaseHex* hex);
	static Factions FindHostileTarget(Factions referenceFaction, ABattleObject* battle);
};


