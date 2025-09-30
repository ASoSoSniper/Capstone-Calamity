// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class Factions : uint8
{
	None UMETA(DisplayName = "None"),
	Human UMETA(DisplayName = "Human"),
	Alien1 UMETA(DisplayName = "Alien 1"),
	Alien2 UMETA(DisplayName = "Alien 2"),
	Alien3 UMETA(DisplayName = "Alien 3"),
	Alien4 UMETA(DisplayName = "Alien 4"),
	Alien5 UMETA(DisplayName = "Alien 5"),
	Alien6 UMETA(DisplayName = "Alien 6")
};

UENUM(BlueprintType)
enum class Race
{
	None,
	Human,
	Alien,
	Robot
};

UENUM(BlueprintType)
enum class FactionRelationship
{
	Neutral,
	Ally,
	Enemy
};
