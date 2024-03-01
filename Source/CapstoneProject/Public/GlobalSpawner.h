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
class AFarmland;
class AMiningStation;
class APowerPlant;
class ACapitalHub;
class AAlienCity;
class UBuildingAttachment;

UENUM()
enum class SpawnableBuildings
{
	None,
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

	SpawnableBuildings buildingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AFarmland* farmland;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) APowerPlant* powerplant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AMiningStation* miningStation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AOutpost* outpost;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int infRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int cavRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int rangedRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int shielderRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int scoutRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int settlerRel;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText infRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText cavRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText rangedRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText shielderRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText scoutRel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText settlerRel;
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
	void BuildAttachment(Factions faction, BuildingAttachments attachment, AOutpost* outpost);

	UClass* DetermineBuildingType(SpawnableBuildings building);
	UClass* DetermineUnitType(SpawnableUnits unit);

	void MergeArmies(ATroop* seeker, ATroop* target, ABaseHex* hex);

	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> miningStationPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> farmlandPrefab;

	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> powerPlantPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> outpostPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> capitalPrefab;
	UPROPERTY(EditAnywhere, Category = "Building") TSubclassOf<class ABuilding> alienCityPrefab;

	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ATroop> troopPrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class AMergedArmy> mergedArmyPrefab;
	UPROPERTY(EditAnywhere, Category = "Unit") TSubclassOf<class ABattleObject> battlePrefab;
	

	UPROPERTY(EditAnywhere) TMap<SpawnableBuildings, FBuildingCost> buildingCosts;
	UPROPERTY(EditAnywhere) TMap<BuildingAttachments, FBuildingCost> attachmentCosts;
	UPROPERTY(EditAnywhere) TMap<SpawnableUnits, FTroopCost> troopCosts;
	UPROPERTY(EditAnywhere) TMap<SpawnableUnits, FTroopStats> troopStats;

	//Hex models
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexModel") TSubclassOf<class UStaticMesh> plainsModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexModel") TSubclassOf<class UStaticMesh> hillsModel;
	void CreateHexModel(TerrainType terrainType, ABaseHex* hex);
};
