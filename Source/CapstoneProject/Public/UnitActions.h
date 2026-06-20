// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FactionEnum.h"
#include "StratResources.h"
#include "TerrainEnum.h"
#include "CoreMinimal.h"
#include "UnitActions.generated.h"

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

class UFaction;
class AFactionController;
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

UCLASS()
class CAPSTONEPROJECT_API UnitActions : public UObject
{
	GENERATED_BODY()

public:

	static bool IsHostileTarget(AMovementAI* unit, AMovementAI* target);
	static bool IsHostileTarget(const EFactions& a, const EFactions& b);

	static bool IsAllyToFaction(EFactionRelationship relationship);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Identity") static UFaction* GetFaction(const EFactions& faction);
	static TMap<EFactions, UFaction*> GetFactions();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Faction Identity") static AFactionController* GetFactionController(const EFactions& faction);
	
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

	static int GetAvailableWorkerType(EFactions faction, WorkerType worker);

	static TArray<int> GetFactionResources(EFactions faction);
	static void SetFactionResources(EFactions faction, EStratResources resourceToChange, int desiredResourceVal);
	static int GetFactionPopulation(EFactions faction);
	static TMap<EStratResources, int> GetMoreSpecificFactionResources(EFactions faction);
	static TMap<EStratResources, int> GetResourcesPerTick(EFactions faction);
	static TMap<EStratResources, int> GetResourceGains(EFactions faction);
	static TMap<EStratResources, int> GetResourceLosses(EFactions faction);
	static TMap<WorkerType, int> GetFactionWorkers(EFactions faction);
	static TMap<WorkerType, int> GetWorkerEnergyCost(EFactions faction);
	static void UpdateResourceCapacity(EFactions faction, int addedCap);

	static int GetResourceCap(EFactions faction);
	static ABaseHex* GetClosestOutpostHex(EFactions faction, AActor* referencePoint);
	static bool HexHasFriendlyTroop(EFactions faction, AActor* hex, ATroop* referenceTroop = nullptr);
	static bool HexHasEnemyTroop(EFactions faction, AActor* hex);
	static int GetFactionStarveLevel(EFactions faction);
	static int GetFactionPowerOutageLevel(EFactions faction);

	static void EnableRobots(EFactions faction, bool enable);
	static void RobotIsActive(EFactions faction, ATroop* robot);
	static void AddResources(EFactions faction, TMap<EStratResources, int> resources);

	static void SetTargetListElement(EFactions faction, AActor* target);
	static void RemoveFromAllTargetLists(ABaseHex* target);
	static TMap<ABaseHex*, EFactions> GetTargetList(EFactions faction);

	static bool ArmyContainsUnit(AMovementAI* troop, UnitTypes type, int& unitIndex);

	static EFactions FindHostileTarget(EFactions referenceFaction, ABaseHex* hex);
	static EFactions FindHostileTarget(EFactions referenceFaction, ABattleObject* battle);
};


