// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitActions.h"
#include "DisplayInfo.generated.h"

#pragma region Troop/Army
USTRUCT(BlueprintType, Blueprintable)
struct FTroopUIData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float progressToMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float healthPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float moralePercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText troopName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
};

USTRUCT(BlueprintType, Blueprintable)
struct FTroopDisplay
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText productionCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText timeToBuild;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText populationCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UnitTypes unitType;
};

USTRUCT(BlueprintType, Blueprintable)
struct FArmyDisplay
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* iconHovered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HPMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int morale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int moraleMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int buildingDamage;
};

USTRUCT(BlueprintType, Blueprintable)
struct FArmyMenuInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HPMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int morale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int moraleMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int buildingDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int vision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int reinforceRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyUpkeepCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int attackvsInfantry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int attackvsCavalry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int attackvsRanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int attackvsShielder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int attackvsScout;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int attackvsSettler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defendvsInfantry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defendvsCavalry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defendvsRanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defendvsShielder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defendvsScout;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defendvsSettler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int infantryNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int cavalryNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int rangedNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int shielderNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int scoutNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int settlerNum;
};

USTRUCT(BlueprintType, Blueprintable)
struct FTroopArmyDisplay
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int morale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxMorale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int dieRoll;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defenderBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* hexIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int infantryQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int cavalryQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int scoutQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int rangedQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int shielderQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int settlerQuantity;
};
#pragma endregion

#pragma region Faction
USTRUCT(BlueprintType, Blueprintable)
struct FFactionDisplay
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString displayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FColor tileColor;
};
USTRUCT(BlueprintType, Blueprintable)
struct FResourceGainLoss
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealthGain = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealthLoss = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyGain = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyLoss = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int foodGain = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int foodLoss = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionGain = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionLoss = 0;
};
USTRUCT(BlueprintType, Blueprintable)
struct FPlayerWorkers
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int available;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int working;
};
USTRUCT(BlueprintType, Blueprintable)
struct FResourcesPerTick
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int production = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int food = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealth = 0;
};
#pragma endregion

#pragma region Building/Siege
USTRUCT(BlueprintType, Blueprintable)
struct FBuildingOnHex
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) SpawnableBuildings buildingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ABuilding* baseBuilding;
};
USTRUCT(BlueprintType, Blueprintable)
struct FBuildingDisplay
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText productionCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText workerCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText buildTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* buildingIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) SpawnableBuildings buildingType;
};

USTRUCT(BlueprintType, Blueprintable)
struct FSiegeBuildingInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float buildingHealthPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int currentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float buildingDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* buildingIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText buildingName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) SpawnableBuildings buildingType;
};
#pragma endregion

#pragma region Hex
USTRUCT(BlueprintType, Blueprintable)
struct FWorkerSliders
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float humanWorkers = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float robotWorkers = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float alienWorkers = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int humanDisplay = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int robotDisplay = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int alienDisplay = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxWorkers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int currWorkers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int availableHumans = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int availableRobots = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int availableAliens = 0;
};
USTRUCT(BlueprintType, Blueprintable)
struct FWorkersInHex
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int humans = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int robots = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int aliens = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxWorkers = 0;
};
#pragma endregion