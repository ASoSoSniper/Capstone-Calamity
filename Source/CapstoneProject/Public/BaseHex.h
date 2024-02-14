// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Components/SphereComponent.h"
#include "TerrainEnum.h"
#include "StratResources.h"
#include "UnitActions.h"
#include "MeshVisibility.h"
#include "BaseHex.generated.h"

class AMergedArmy;
class AMovementAI;
class ABuilding;
class AGlobalSpawner;

USTRUCT(BlueprintType)
struct FHexInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int food;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int production;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defenderBonus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float moveMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float attritionMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int visionModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
};

USTRUCT(BlueprintType)
struct FHexDisplay
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText food;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText production;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText energy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText workerCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defenderBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText moveMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attritionBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText visionModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
};


UCLASS()
class CAPSTONEPROJECT_API ABaseHex : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseHex();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//Components created upon initialization
#pragma region Components

	//UPROPERTY(VisibleAnywhere) UHexInfo* hexInfo;
	UPROPERTY(VisibleAnywhere, Category = "Components") UInteractable* interactable;
	UPROPERTY(VisibleAnywhere, Category = "Components") UMeshVisibility* visibility;
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* troopAnchor;
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* buildingAnchor;

	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMeshAttachment;

#pragma endregion

//Variables that change depending on the hex's interaction with the world
#pragma region Variables

	UPROPERTY(EditAnywhere) bool isSelected = false;
	UPROPERTY(EditAnywhere) bool hasFOW = true;

	UPROPERTY(VisibleAnywhere) ABuilding* building;

	UPROPERTY(VisibleAnywhere) TArray<AMovementAI*> troopsInHex;

#pragma endregion

//Variables that change depending on the hex's identity
#pragma region Identity
	struct ResourceStats
	{
		int yieldBonus;
	};

	TMap<StratResources, ResourceStats> resourceBonuses;

	UPROPERTY(EditAnywhere, Category = "Resources") int energyYieldBonus = 1;
	UPROPERTY(EditAnywhere, Category = "Resources") int productionYieldBonus = 1;
	UPROPERTY(EditAnywhere, Category = "Resources") int foodYieldBonus = 1;
	UPROPERTY(EditAnywhere, Category = "Identity") int hexID;
	UPROPERTY(EditAnywhere, Category = "Identity") TerrainType hexTerrain = TerrainType::Plains;
	TerrainType terrainChange = TerrainType::None;
	UPROPERTY(EditAnywhere, Category = "Identity") Factions hexOwner = Factions::None;

	UPROPERTY(EditAnywhere, Category = "Identity") TMap<TerrainType, FHexInfo> hexInfo;
#pragma endregion

	TArray<AActor*> GetObjectsInHex();

	void CheckForHostility(AMovementAI* refTroop);
	void AddTroopToHex(AMovementAI* troop);
	void RemoveTroopFromHex(AMovementAI* troop);

	void BeginBattle();

	AGlobalSpawner* spawner;

	bool battleInProgress;

	TMap<WorkerType, int> workersInHex;
	int maxWorkers = 10;

	float currentHarvestTime;
	float maxHarvestTime = 2.f;
	bool harvesting;

	bool ActiveHarvesting();

	void UpdateFoodYield(int value);
	void UpdateProductionYield(int value);
	void UpdateEnergyYield(int value);

	void ToggleResourceYield();

	void RequestTerrainChange(bool modelOnly = false);

	FHexDisplay GetDisplayInfo();

	ABaseHex* FindFreeAdjacentHex(Factions faction, TArray<ABaseHex*> ignoredHexes);

	void SetFaction(Factions faction);
};
