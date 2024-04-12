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

USTRUCT(BlueprintType, Blueprintable)
struct FTroopUIData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float progressToMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float healthPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float moralePercent;
};

USTRUCT(BlueprintType, Blueprintable)
struct FUnitComposition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int quantity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float compPercent = 0.f;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float unrestMod;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyUpkeepCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int maxWorkers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* buildingIcon;
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
struct FBuildingOnHex
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) SpawnableBuildings buildingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AFarmland* farmland;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) APowerPlant* powerplant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AMiningStation* miningStation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AOutpost* outpost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ACapitalHub* hub;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ABuilding* baseBuilding;
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
struct FTroopStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText desc;
	UnitTypes type;
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
USTRUCT(BlueprintType, Blueprintable)
struct FTroopQuantity
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UnitTypes unitType = UnitTypes::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int quantity = 0;
};

USTRUCT(BlueprintType, Blueprintable)
struct FAttachmentBuildProgress
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float currentProgress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float buildTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool isBuilding;
};

USTRUCT(BlueprintType, Blueprintable)
struct FAttachmentTTBuildInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int workerCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int buildTime;
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
	AActor* SpawnSmoke(AActor* object);

	bool PurchaseTroop(Factions faction, UnitTypes unit, AOutpost* outpost);
	ATroop* BuildTroop(Factions faction, UnitTypes unit, ABaseHex* hex);
	ATroop* BuildArmy(Factions faction, ABaseHex* hex);
	void BuildAttachment(Factions faction, BuildingAttachments attachment, AOutpost* outpost);

	UClass* DetermineBuildingType(SpawnableBuildings building);

	void MergeArmies(ATroop* seeker, ATroop* target, ABaseHex* hex);

	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> miningStationPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> farmlandPrefab;

	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> powerPlantPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> outpostPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> capitalPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> alienCityPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> rockCityPrefab;

	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ATroop> troopPrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class AMergedArmy> mergedArmyPrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ABattleObject> battlePrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ASiegeObject> siegePrefab;
	UPROPERTY(EditAnywhere, Category = "Particle Effect") TSubclassOf<class AActor> smokePrefab;
	

	UPROPERTY(EditAnywhere) TMap<SpawnableBuildings, FBuildingCost> buildingCosts;
	UPROPERTY(EditAnywhere) TMap<BuildingAttachments, FBuildingCost> attachmentCosts;
	UPROPERTY(EditAnywhere) TMap<SpawnableBuildings, FBuildingStats> buildingStats;
	UPROPERTY(EditAnywhere) TMap<BuildingAttachments, FBuildingStats> attachmentStats;
	UPROPERTY(EditAnywhere) TMap<UnitTypes, FTroopCost> troopCosts;
	UPROPERTY(EditAnywhere) TMap<UnitTypes, FTroopStats> troopStats;
	UPROPERTY(EditAnywhere) TMap<TerrainType, FVisibilityMaterials> terrainTileMaterials;
	UPROPERTY(EditAnywhere) TMap<Factions, FTroopMaterials> troopFactionMaterials;

	//Hex models
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexModel") TSubclassOf<class UStaticMesh> plainsModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexModel") TSubclassOf<class UStaticMesh> hillsModel;
	void CreateHexModel(TerrainType terrainType, ABaseHex* hex);

	TArray<TArray<ABaseHex*>> hexArray;
	TArray<ABaseHex*> alienHexes;
	void ProceduralHexGen(int numHexs, ShapesOfMap shape);
	void SpawnBuildingsAroundCity(ABaseHex* centerHex);
	UPROPERTY(EditAnywhere) int buildingDistanceFromCity = 2;
	FVector2D GetHexCoordinates(ABaseHex* hex);
	ABaseHex* GetHexFromCoordinates(int x, int y);
	bool BuildingOnHex(ABaseHex* hex);
	UPROPERTY(EditAnywhere) TSubclassOf<class ABaseHex> hexActor;
	ABasePlayerController* controller;

	UnitActions::UnitData CreateTroopUnitData(Factions faction, UnitTypes unitType);
};
