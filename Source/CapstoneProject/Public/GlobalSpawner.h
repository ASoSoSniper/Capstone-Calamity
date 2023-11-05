// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitActions.h"
#include "GlobalSpawner.generated.h"

class AMergedArmy;
class ABattleObject;
UENUM()
enum class SpawnableBuildings
{
	Capitol,
	MiningStation,
	Farmland,
	PowerPlant,
	Storage,
	Road,
	Outpost,
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

	UClass* DetermineBuildingType(SpawnableBuildings building);
	UClass* DetermineUnitType(SpawnableUnits unit);

	void MergeArmies(ATroop* seeker, ATroop* target, ABaseHex* hex);

	UPROPERTY(EditAnywhere) TSubclassOf<class ABuilding> miningStationPrefab;
	UPROPERTY(EditAnywhere) TSubclassOf<class ABuilding> farmlandPrefab;

	UPROPERTY(EditAnywhere) TSubclassOf<class ABuilding> powerPlantPrefab;

	UPROPERTY(EditAnywhere) TSubclassOf<class ATroop> troopPrefab;
	UPROPERTY(EditAnywhere) TSubclassOf<class AMergedArmy> mergedArmyPrefab;
	UPROPERTY(EditAnywhere) TSubclassOf<class ABattleObject> battlePrefab;
};
