// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class TerrainType : uint8
{
	None,
	Plains, 
	Hills, 
	Mountains,
	Jungle,
	SporeField,
	Border,
	Forest,
	Ship,
	AlienCity,
	TheRock
};
