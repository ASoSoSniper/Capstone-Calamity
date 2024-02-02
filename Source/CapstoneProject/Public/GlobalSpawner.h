// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitActions.h"
#include "TerrainEnum.h"
#include "GlobalSpawner.generated.h"

class AMergedArmy;
class ABattleObject;
class ASettler;
class AOutpost;
UENUM()
enum class SpawnableBuildings
{
	MiningStation,
	Farmland,
	PowerPlant,
	Road,
	Outpost
};

UENUM()
enum class BuildingAttachments
{
	Storage,
	DefenseStation,
	RobotFactory,
	RobotBarracks,
	TradeOutpost,
	Embassy,
	PoliceStation
};

UENUM()
enum class SpawnableUnits
{
	None,
	Cavalry,
	Infantry,
	Ranged,
	Shielder,
	Scout,
	Settler,
	Army
};

USTRUCT(BlueprintType, Blueprintable)
struct FBuildingCost
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int workerCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int timeToBuild = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* buildingIcon;
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
};
USTRUCT(BlueprintType, Blueprintable)
struct FTroopCost
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int timeToBuild = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int populationCost = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
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
};

USTRUCT(BlueprintType, Blueprintable)
struct FCuedTroop
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FTroopCost troopInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float currentTime;
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

UCLASS()
class CAPSTONEPROJECT_API AGlobalSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobalSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnBuilding(Factions faction, SpawnableBuildings building, ABaseHex* hex);
	ATroop* SpawnTroop(ABaseHex* hex, UnitActions::UnitData data, float parentHealthPercent = 1.f);
	AMergedArmy* SpawnArmy(ABaseHex* hex, TArray<UnitActions::UnitData> groupData, float parentHealthPercent = 1.f);
	ABattleObject* SpawnBattle(ABaseHex* hex);

	bool PurchaseTroop(Factions faction, SpawnableUnits unit, AOutpost* outpost);
	void BuildTroop(Factions faction, SpawnableUnits unit, ABaseHex* hex, AOutpost* outpost = nullptr);

	UClass* DetermineBuildingType(SpawnableBuildings building);
	UClass* DetermineUnitType(SpawnableUnits unit);

	void MergeArmies(ATroop* seeker, ATroop* target, ABaseHex* hex);

	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> miningStationPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> farmlandPrefab;

	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> powerPlantPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> outpostPrefab;

	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ATroop> troopPrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class AMergedArmy> mergedArmyPrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ABattleObject> battlePrefab;
	

	UPROPERTY(EditAnywhere) TMap<SpawnableBuildings, FBuildingCost> buildingCosts;
	UPROPERTY(EditAnywhere) TMap<BuildingAttachments, FBuildingCost> attachmentCosts;
	UPROPERTY(EditAnywhere) TMap<SpawnableUnits, FTroopCost> troopCosts;

	//Hex models
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexModel") TSubclassOf<class UStaticMesh> plainsModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexModel") TSubclassOf<class UStaticMesh> hillsModel;
	void CreateHexModel(TerrainType terrainType, ABaseHex* hex);
};
