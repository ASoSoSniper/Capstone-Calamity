// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FactionEnum.h"
#include "StratResources.h"
#include "TerrainEnum.h"
#include "CoreMinimal.h"

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
enum class ActionStates
{
	None,
	HexManage,
	BaseManage,
	TroopManage,
	BattleManage
};
UENUM()
enum class UnitTypes
{
	None,
	Cavalry,
	Infantry,
	Ranged,
	Shield,
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

class ABaseHex;
class ATroop;
class ASettler;
class AMovementAI;
class ABuilding;
class UUnitStats;
class ABattleObject;
class AOutpost;

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
		int defense;

		int speed;
		int currentMorale;
		int maxMorale;
		int minDamage;
		int maxDamage;
		int reinforceRate;

		TArray<UnitData> savedUnits;
	};

	static UnitData CollectUnitData(UUnitStats* unit);

	static int GetAvailableWorkerType(Factions faction, WorkerType worker);

	static int AddWorkers(Factions faction, WorkerType worker, int& desiredWorkers, ABaseHex* hex);
	static int RemoveWorkers(Factions faction, WorkerType worker, int& desiredWorkers, ABaseHex* hex);
	static TArray<int> GetFactionResources(Factions faction);
	static int GetFactionPopulation(Factions faction);
	static TMap<StratResources, int> GetMoreSpecificFactionResources(Factions faction);
	static TMap<StratResources, int> GetResourcesPerTick(Factions faction);
	static TMap<WorkerType, int> GetFactionWorkers(Factions faction);
	static void ConsumeSpentResources(Factions faction, TMap<StratResources, int> resources, TMap<WorkerType, int> workers, ABaseHex* hex = nullptr, AOutpost* outpost = nullptr);
	static void ConsumeSpentResources(Factions faction, TArray<int> values);

	static TArray<TerrainType> GetNonBuildableTerrain();
	static int GetResourceCap(Factions faction);
	static ABaseHex* GetClosestOutpostHex(Factions faction, AActor* referencePoint);
	static int GetFactionStarveLevel(Factions faction);
};


