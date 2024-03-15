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
	SporeField,
	Forest,

	Jungle,
	Mountains,

	Ship,
	AlienCity,
	TheRock,
	Border
};
