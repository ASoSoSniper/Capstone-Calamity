// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FactionEnum.h"
#include "StratResources.h"
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
	Building
};
UENUM()
enum class ActionStates
{
	None,
	HexManage,
	BaseManage,
	TroopManage
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
	Settler
};

class ABaseHex;
class ATroop;
class AMovementAI;
class ABuilding;
class UUnitStats;

class CAPSTONEPROJECT_API UnitActions
{
public:
	
	static void HarvestResources(Factions faction, int quantity, StratResources resource);

	static void Attack(TArray<AActor*> attacker, TArray<AActor*> target);

	static bool IsHostileTarget(AMovementAI* unit, AActor* target);

	static void AssignFaction(Factions faction, AActor* target);

	
	struct SelectionIdentity
	{
		AActor* actor;
		ABaseHex* hex;
		ATroop* moveAI;
		ABuilding* building;
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
		int minDamage;
		int maxDamage;
	};

	static UnitData CollectUnitData(UUnitStats* unit);
};


