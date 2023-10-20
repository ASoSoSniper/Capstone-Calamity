// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "Building.h"

/**
 * 
 */
class CAPSTONEPROJECT_API Faction
{
public:
	Faction();
	~Faction();

	Factions faction;

	TMap<StratResources, int> resourceInventory;
	TArray<ATroop*> allUnits;
	TArray<ABuilding*> allBuildings;

	TMap<Factions, FactionRelationship> factionRelationships;

	void FindActiveFactions();
};
