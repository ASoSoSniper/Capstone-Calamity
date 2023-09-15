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
class CAPSTONEPROJECT_API UnitActions
{
public:
	
	static void HarvestResources(Factions faction, int quantity, StratResources resource);

	static void Attack(TArray<AActor*> attacker, TArray<AActor*> target);

	static bool IsHostileTarget(AMovementAI* unit, AActor* target);

	static void AssignFaction(Factions faction, AActor* target);
};
