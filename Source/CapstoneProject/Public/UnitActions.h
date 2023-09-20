// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FactionEnum.h"
#include "BaseHex.h"
#include "MovementAI.h"
#include "Building.h"
#include "UnitStats.h"
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
	BaseManage,
	TroopManage
};

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
		AMovementAI* moveAI;
		ABuilding* building;
		ObjectTypes type;
	};

	static SelectionIdentity DetermineObjectType(AActor* object);
};


