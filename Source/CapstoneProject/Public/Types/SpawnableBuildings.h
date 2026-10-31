// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

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