// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FactionEnum.h"
#include "StratResources.h"
#include "TerrainEnum.h"
#include "CoreMinimal.h"
//#include "GlobalSpawner.h"

/**
 * 
 */

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
	Road,
	Outpost,
	Capitol,
	AlienCity,
	RockCity
};

UENUM(BlueprintType)
enum class BuildingAttachments : uint8
{
	Storage,
	DefenseStation,
	RobotFactory,
	RobotBarracks,
	TradeOutpost,
	Embassy,
	PoliceStation
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
class UBuildingAttachment;
class AGlobalSpawner;
struct FTroopStats;
struct FUnitComposition;

class CAPSTONEPROJECT_API UnitActions
{
public:
	
	static void HarvestResources(Factions faction, int quantity, StratResources resource);

	static void Attack(TArray<AActor*> attacker, TArray<AActor*> target);

	static bool IsHostileTarget(AMovementAI* unit, AMovementAI* target);
	static bool IsHostileTarget(AMovementAI* unit, AActor* target);

	static bool IsAllyToFaction(FactionRelationship relationship);

	static EngagementSelect DetermineConflictAlignment(Factions& unitFaction, TArray<Factions>& group1, TArray<Factions>& group2);

	static FactionRelationship GetFactionRelationship(Factions unitFaction, Factions targetFaction);

	static void AssignFaction(Factions faction, AActor* target);
	static void AssignFaction(Factions faction, ABaseHex* hex);
	static void RemoveFromFaction(Factions faction, AActor* target);

	static Faction* GetFaction(Factions faction);
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

	struct UnitData
	{
		Factions faction;
		UnitTypes unitType;
		bool upgraded;

		int currentHP;
		int maxHP;
		int currentMorale;
		int maxMorale;

		int vision;
		int speed;

		int damage;
		int siegePower;

		int reinforceRate;
		int energyUpkeep;

		TArray<UnitData> savedUnits;
	};

	static UnitData CollectUnitData(UUnitStats* unit);
	static void ApplyDataToUnitStats(UUnitStats* unit, UnitData& data);
	static void ApplyDataToUnitStats(UUnitStats* unit, FTroopStats& data);
	static void AddUnitData(UUnitStats* unit, UnitData& data);
	static UnitData AddUnitData(UnitData& unit, UnitData& data);

	static int GetAvailableWorkerType(Factions faction, WorkerType worker);

	static int AddWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex);
	static int AddWorkers(Factions faction, WorkerType worker, int desiredWorkers, AOutpost* outpost, BuildingAttachments attachment);
	static int RemoveWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex);
	static int RemoveWorkers(Factions faction, WorkerType worker, int desiredWorkers, AOutpost* outpost, BuildingAttachments attachment);
	static bool SetWorkers(Factions faction, WorkerType worker, int desiredWorkers);
	static int SetWorkers(Factions faction, WorkerType worker, int desiredWorkers, ABaseHex* hex);
	static int SetWorkers(Factions faction, WorkerType worker, int desiredWorkers, AOutpost* outpost, BuildingAttachments attachment);

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
	static void ConsumeSpentResources(Factions faction, TMap<StratResources, int> resources, TMap<WorkerType, int> workers, AOutpost* outpost, BuildingAttachments attachment);
	static void ConsumeSpentResources(Factions faction, TArray<int> values);
	static void UpdateResourceCapacity(Factions faction, int addedCap);

	static TArray<TerrainType> GetNonBuildableTerrain();
	static bool HexIsTraversable(AActor* hex);
	static int GetResourceCap(Factions faction);
	static ABaseHex* GetClosestOutpostHex(Factions faction, AActor* referencePoint);
	static bool HexHasFriendlyTroop(Factions faction, AActor* hex);
	static int GetFactionStarveLevel(Factions faction);
	static int GetFactionPowerOutageLevel(Factions faction);

	static void EnableRobots(Factions faction, bool enable);
	static void RobotIsActive(Factions faction, ATroop* robot);
	static void AddResources(Factions faction, TMap<StratResources, int> resources);

	static TMap<UnitTypes, FUnitComposition> GetArmyComposition(ATroop* army);
	static UnitTypes GetLargestUnitQuantity(ATroop* army);

	static void SetTargetListElement(Factions faction, AActor* target, bool addToList);
	static void RemoveFromAllTargetLists(AActor* target);
	static TArray<AActor*> GetTargetList(Factions faction);
};


