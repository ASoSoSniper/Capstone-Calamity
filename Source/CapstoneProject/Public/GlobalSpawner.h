// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitActions.h"
#include "TerrainEnum.h"
#include "GlobalSpawner.generated.h"

class ABasePlayerController;
class AMergedArmy;
class ABattleObject;
class ASiegeObject;
class ASettler;
class AOutpost;
class AFarmland;
class AMiningStation;
class APowerPlant;
class ACapitalHub;
class AAlienCity;
class ARockCity;
class UBuildingAttachment;

#pragma region UI
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
#pragma region Visuals
UENUM(BlueprintType)
enum class ShapesOfMap
{
	None,
	Square,
	Rectangle,
	Catan,
	Circle
};
USTRUCT(BlueprintType, Blueprintable)
struct FCinematicObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) AActor* object;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float travelTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float lingerTime;
};
USTRUCT(BlueprintType, Blueprintable)
struct FVisibilityMaterials
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* hiddenTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* visibleTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* selectedTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* modelHiddenTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* modelVisibleTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* modelSelectedTexture;
};
USTRUCT(BlueprintType, Blueprintable)
struct FTroopMaterials
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* visibleTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* selectedTexture;
};
#pragma endregion
#pragma region Factions
USTRUCT(BlueprintType, Blueprintable)
struct FFactionDisplay
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString displayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FColor tileColor;
};
USTRUCT(BlueprintType, Blueprintable)
struct FFactionPersonality
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int xenophilia;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int spirituality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int compliance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int violence;
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
#pragma endregion
#pragma region Hexes
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
USTRUCT(BlueprintType)
struct FPointOfInterest
{
	GENERATED_USTRUCT_BODY()

private:
	UPROPERTY(EditAnywhere) FString pointTitle = TEXT("Hidden Cache");
	UPROPERTY(EditAnywhere) TMap<EStratResources, int32> rewards;
	UPROPERTY(EditAnywhere) int32 daysToComplete = 1;
	int32 hoursRemaining = 0;
	int32 minutes = 0;
public:
	FPointOfInterest() {};
	FPointOfInterest(FPointOfInterest& setPOI)
	{
		pointTitle = setPOI.pointTitle;
		rewards = setPOI.rewards;
		daysToComplete = setPOI.daysToComplete;
	}
	FPointOfInterest(TMap<EStratResources, int32> setRewards, FString setTitle = TEXT("Hidden Cache"), int32 setDays = 1) :
		pointTitle(setTitle), rewards(setRewards), daysToComplete(setDays) {
	}

	FString GetPointTitle() const;
	const TMap<EStratResources, int32>& GetRewards() const;
	FString GetWorldDisplay() const;
	int32 GetDaysToComplete() const;
	bool Work();
	bool WorkCompleted() const;
};
#pragma endregion
#pragma region Buildings
USTRUCT(BlueprintType, Blueprintable)
struct FBuildingOnHex
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) SpawnableBuildings buildingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ABuilding* baseBuilding;
};
USTRUCT(BlueprintType, Blueprintable)
struct FBuildingCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int workerCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int timeToBuild = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int hexLayers = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* buildingIcon;
};
USTRUCT(BlueprintType, Blueprintable)
struct FBuildingStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int foodYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealthYield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int resourceCapIncrease;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int robotStorage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float diplomacy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float trade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int siegeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int vision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float unrestMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyUpkeepCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxWorkers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* buildingIcon;
};

USTRUCT(BlueprintType, Blueprintable)
struct FRobotFactoryStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyCost;
};
USTRUCT(BlueprintType, Blueprintable)
struct FRobotStorageStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxRobotStorage;
};
USTRUCT(BlueprintType, Blueprintable)
struct FMaterialStorageStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int resourceIncrease;
};
USTRUCT(BlueprintType, Blueprintable)
struct FDefenseStationStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HP;
};
#pragma endregion

#pragma region Troops and Armies
USTRUCT(BlueprintType, Blueprintable)
struct FTroopCost
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int timeToBuild = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int populationCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
};
USTRUCT(BlueprintType, Blueprintable)
struct FTroopStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText desc;
	UnitTypes type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* iconHovered;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int siegePower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int vision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int morale;
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
};
USTRUCT(BlueprintType, Blueprintable)
struct FUnitComposition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int quantity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float compPercent = 0.f;
};

USTRUCT(BlueprintType, Blueprintable)
struct FCuedTroop
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FTroopCost troopInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float currentTime;
};

USTRUCT(BlueprintType, Blueprintable)
struct FStatusEffect
{
	GENERATED_USTRUCT_BODY()

public:
	FStatusEffect() {};
	FStatusEffect(FString name, Factions faction, FactionRelationship affected, float hp, float morale, int vision, float speed, float damage, float siegePower) :
		effectName(name), originFaction(faction), factionsToAffect(affected),
		hpMod(hp), moraleMod(morale), visionMod(vision), speedMod(speed), damageMod(damage), siegePowerMod(siegePower) {		
	};

	FString GetEffectName() const;
	Factions GetOriginFaction() const;
	FactionRelationship GetFactionsToAffect() const;

	float GetHPMod() const;
	float GetMoraleMod() const;
	int GetVisionMod() const;
	float GetSpeedMod() const;
	float GetDamageMod() const;
	float GetSiegePowerMod() const;

private:
	FString effectName = TEXT("");
	Factions originFaction = Factions::None;
	FactionRelationship factionsToAffect = FactionRelationship::Neutral;

	float hpMod = 1.f;
	float moraleMod = 1.f;

	int visionMod = 0;
	float speedMod = 1.f;

	float damageMod = 1.f;
	float siegePowerMod = 1.f;
};

USTRUCT(BlueprintType, Blueprintable)
struct FUnitData
{
	GENERATED_USTRUCT_BODY()

public:
	FUnitData() : faction(Factions::None), unitType(UnitTypes::None) {};
	FUnitData(Factions setFaction) : faction(setFaction), unitType(UnitTypes::None)
	{
		FUnitData(setFaction, UnitTypes::None);
	};
	FUnitData(Factions setFaction, UnitTypes setUnitType) : faction(setFaction), unitType(setUnitType)
	{
		if (unitType == UnitTypes::Army)
			GenerateArmyName();
	}

	TMap<UnitTypes, FUnitComposition> GetUnitComposition() const;
	UnitTypes GetLargestUnitQuantity() const;

	Factions GetFaction() const;
	UnitTypes GetUnitType() const;

	void SetUnitValues(int setHealth, int setMorale, int setVision, int setSpeed, int setDamage, int setSiegePower, int setReinforceRate, int setEnergyUpKeep);
	void AddUnitData(FUnitData* data);
	FUnitData* ExtractUnitData(int32 index, bool killOnExtraction = false);
	void SetBuildingValues(int setHealth, int setVision, int setSiegePower, int setEnergyUpkeep);

	void GenerateArmyName(FString newName = TEXT(""));
	FText GetArmyName() const;
	FString GetNameRaw() const;
	int GetNameInstance() const;

	int GetCurrentHP() const;
	int GetMaxHP() const;
	void SetHPByAlpha(float alpha, bool allowDeath = true);
	int GetCurrentMorale() const;
	int GetMaxMorale() const;
	void SetMoraleByAlpha(float alpha);
	void SetMoraleByValue(int value);
	float GetHPAlpha() const;
	float GetMoraleAlpha() const;
	bool IsAlive() const;
	bool HasMorale() const;

	int DamageHP(int amount);
	int DamageMorale(int amount);
	int HealHP(int amount);

	int GetVision() const;
	int GetSpeed() const;

	int GetDamage() const;
	int GetSiegePower() const;

	int GetReinforceRate() const;
	int GetEnergyUpkeep() const;

	bool IsBuilding() const;

	const TArray<FUnitData*>& GetSavedUnits() const;
	int GetSavedUnitCount() const;

	bool SetupComplete() const;
	void DestroyWorldData();

	const TSet<FStatusEffect*>& GetStatusEffects() const;
	void AddStatusEffect(FStatusEffect* effect);
	void RemoveStatusEffect(FStatusEffect* effect);
	void ClearStatusEffects();

private:
	Factions faction;
	UnitTypes unitType;

	FString armyName = TEXT("");
	int nameInstance = 0;

	int currentHP = 0;
	int maxHP = 0;
	int currentMorale = 0;
	int maxMorale = 0;

	int vision = 0;
	int speed = 0;

	int damage = 0;
	int siegePower = 0;

	int reinforceRate = 0;
	int energyUpkeep = 0;

	bool isBuilding = false;

	TArray<FUnitData*> savedUnits;
	TSet<FStatusEffect*> statusEffects;

	void RemoveArmyName();
};
#pragma endregion

UCLASS()
class CAPSTONEPROJECT_API AGlobalSpawner : public AActor
{
	GENERATED_BODY()
	
#pragma region General Logic
public:	
	AGlobalSpawner();
	virtual void Tick(float DeltaTime) override;

	FFactionDisplay* GetFactionDisplayPreset(Factions faction);

	static AGlobalSpawner* spawnerObject;
	ABasePlayerController* controller;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Factions") TMap<Factions, FFactionDisplay> factionDisplayPresets;
#pragma endregion

#pragma region Hex Generation
public:
	void CreateHexModel(TerrainType terrainType, ABaseHex* hex);
	ABaseHex* GetHexFromCoordinates(int x, int y);

	TArray<TArray<ABaseHex*>> hexArray;
private:
	void ProceduralHexGen(int numHexs, ShapesOfMap shape);
	void SpawnBuildingsAroundCity(ABaseHex* centerHex);
	void SpawnPointsOfInterest();

	UPROPERTY(EditAnywhere, Category = "World Generation") TSubclassOf<class ABaseHex> hexActor;
	UPROPERTY(EditAnywhere, Category = "World Generation") int pointOfInterestCount = 15;
	UPROPERTY(EditAnywhere, Category = "World Generation") TArray<FPointOfInterest> pointsOfInterest;
	UPROPERTY(EditAnywhere, Category = "World Generation") int hexSeedSize = 5;
	UPROPERTY(EditAnywhere, Category = "World Generation") int hexRandToMaintain = 5;
	UPROPERTY(EditAnywhere, Category = "World Generation") int buildingDistanceFromCity = 2;
#pragma endregion

#pragma region Building Construction
public:	
	void SpawnBuilding(Factions faction, SpawnableBuildings building, ABaseHex* hex);
	void SpawnBuildingFree(Factions faction, SpawnableBuildings building, ABaseHex* hex, bool buildAtStart = false);
	UClass* DetermineBuildingType(SpawnableBuildings building);

	UPROPERTY(EditAnywhere) TMap<SpawnableBuildings, FBuildingCost> buildingCosts;
	UPROPERTY(EditAnywhere) TMap<SpawnableBuildings, FBuildingStats> buildingStats;
private:
	bool BuildingOnHex(ABaseHex* hex);

	UPROPERTY(EditAnywhere, Category = "Building") TMap<SpawnableBuildings, TSubclassOf<class ABuilding>> buildingPrefabs;
#pragma endregion

#pragma region Troop Construction
public:
	//Spawn an army given the unit data of an existing army without a physical form. 
	//Use when an army exits a battle or storage building, where identity preservation is important.
	ATroop* SpawnArmyByUnit(ABaseHex* hex, FUnitData* data, float parentHealthPercent = 1.f);

	//Spawn an army from an array of individual troops, where prior identity of the group is NOT important.
	//Use when splitting an army, in half or otherwise.
	AMergedArmy* SpawnArmyByArray(ABaseHex* hex, TArray<FUnitData*> groupData, float parentHealthPercent = 1.f);

	void MergeArmies(ATroop* seeker, ATroop* target, ABaseHex* hex);

	bool PurchaseTroop(Factions faction, UnitTypes unit);
	ATroop* BuildTroop(Factions faction, UnitTypes unit, ABaseHex* hex);

	UPROPERTY(EditAnywhere) TMap<UnitTypes, FTroopCost> troopCosts;
	UPROPERTY(EditAnywhere) TMap<UnitTypes, FTroopStats> troopStats;
private:
	ATroop* BuildArmy(Factions faction, ABaseHex* hex);

	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ATroop> troopPrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class AMergedArmy> mergedArmyPrefab;
#pragma endregion

#pragma region Battles
public:
	ABattleObject* SpawnBattle(ABaseHex* hex);
private:
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ABattleObject> battlePrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ASiegeObject> siegePrefab;
#pragma endregion

#pragma region Particle Effects
public:
	AActor* SpawnSmoke(AActor* object);
	AActor* SpawnEndParticle(AActor* object, GameStates state);
private:
	UPROPERTY(EditAnywhere, Category = "Particle Effect") TSubclassOf<class AActor> smokePrefab;
	UPROPERTY(EditAnywhere, Category = "Particle Effect") TSubclassOf<class AActor> explosionPrefab;
	UPROPERTY(EditAnywhere, Category = "Particle Effect") TSubclassOf<class AActor> fireWorksPrefab;
#pragma endregion
};
