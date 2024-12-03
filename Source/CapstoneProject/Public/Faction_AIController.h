// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalSpawner.h"
#include "Faction.h"

class FactionAction;

UENUM()
enum class EFactionPriority : uint8
{
	Expansion,
	Diplomacy,
	ResourceGain,
	Defense
};

class CAPSTONEPROJECT_API Faction_AIController
{
public:
	Faction_AIController(Faction* faction);
	~Faction_AIController();

	Faction* GetFaction();

	void SelectAction();

	TArray<FactionAction*> actions;

private:
	Faction* parentFaction;

	FFactionPersonality personality;
	void GeneratePersonality();
};
