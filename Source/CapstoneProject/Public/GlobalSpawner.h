// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitActions.h"
#include "TerrainEnum.h"
#include "Types/DisplayInfo.h"
#include "GlobalSpawner.generated.h" // Alex Code - includes here and below

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
class UHexBiomeMaskComponent; // Alex Code - class
struct FUnitData;

#pragma region Buildings
USTRUCT(BlueprintType, Blueprintable)
struct FBuildingCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int workerCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int timeToBuild = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EBuildingSize size = EBuildingSize::OneTile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* buildingIcon;
};
USTRUCT(BlueprintType, Blueprintable)
struct FBuildingStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EStratResources, int32> resourceYields;

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
struct FCuedTroop
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FTroopCost troopInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float currentTime;
};
#pragma endregion

UCLASS(HideCategories = (Rendering, Replication, Input, Actor, LOD, HLOD, Collision, Physics, Cooking, Events, WorldPartition, Events, DataLayers))
class CAPSTONEPROJECT_API AGlobalSpawner : public AActor
{
	GENERATED_BODY()
	
#pragma region General Logic
public:	
	AGlobalSpawner();

	static AGlobalSpawner* spawnerObject;
	ABasePlayerController* controller;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Building Construction
public:	
	bool SpawnBuilding(EFactions faction, SpawnableBuildings building, ABaseHex* hex);
	bool SpawnOutpost(EFactions faction, const TArray<ABaseHex*>& hexes);
	void SpawnBuildingFree(EFactions faction, SpawnableBuildings building, ABaseHex* hex, bool buildAtStart = false);
	UClass* DetermineBuildingType(SpawnableBuildings building);
	bool CanAffordBuilding(UFaction* faction, SpawnableBuildings building) const;

	static EStratResources GetMainBuildingYield(SpawnableBuildings building);

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

	bool PurchaseTroop(EFactions faction, UnitTypes unit);
	ATroop* BuildTroop(EFactions faction, UnitTypes unit, ABaseHex* hex);

	UPROPERTY(EditAnywhere) TMap<UnitTypes, FTroopCost> troopCosts;
	UPROPERTY(EditAnywhere) TMap<UnitTypes, FTroopStats> troopStats;
private:
	ATroop* BuildArmy(EFactions faction, ABaseHex* hex);

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
