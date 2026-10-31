// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TooltipInfo.generated.h"


USTRUCT(BlueprintType, Blueprintable)
struct FTroopTTInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText titleTT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText descTT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText siegePower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText vision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText morale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText reinforceRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText energyUpkeepCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attackvsInfantry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attackvsCavalry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attackvsRanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attackvsShielder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attackvsScout;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attackvsSettler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defendvsInfantry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defendvsCavalry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defendvsRanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defendvsShielder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defendvsScout;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defendvsSettler;
};

USTRUCT(BlueprintType, Blueprintable)
struct FBuildingTTInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText titleTT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText descTT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText energyMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText foodMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText prodMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText wealthMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText resourceStorageMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText robotStorageMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText diploMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText tradeMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText siegeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText siegeHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText unrestMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText energyUpkeepCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText maxWorkers;
};