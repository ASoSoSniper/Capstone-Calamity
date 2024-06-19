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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealth;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText wealth;

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

	UPROPERTY(VisibleAnywhere, Category = "Components") UInteractable* interactable;
	UPROPERTY(VisibleAnywhere, Category = "Components") UMeshVisibility* visibility;
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* troopAnchor;
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* buildingAnchor;

	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMeshAttachment;
	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexBase;
	UPROPERTY(VisibleAnywhere, Category = "Components") UAudioComponent* audioComponent;

#pragma endregion

//Variables that change depending on the hex's interaction with the world
#pragma region Variables

	UPROPERTY(EditAnywhere) bool isSelected = false;
	UPROPERTY(EditAnywhere) bool hasFOW = true;

	UPROPERTY(VisibleAnywhere) ABuilding* building;

	UPROPERTY(VisibleAnywhere) TArray<AMovementAI*> troopsInHex;

	UPROPERTY(VisibleAnywhere) ABattleObject* battle;

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
	UPROPERTY(EditAnywhere, Category = "Resources") int wealthYieldBonus = 1;
	UPROPERTY(EditAnywhere, Category = "Identity") int hexID;
	UPROPERTY(EditAnywhere, Category = "Identity") Factions hexOwner = Factions::None;

	UPROPERTY(EditAnywhere, Category = "Identity") TMap<TerrainType, FHexInfo> hexInfo;

	UPROPERTY(EditAnywhere, Category = "Identity") float moveMultiplier = 1.f;
	UPROPERTY(EditAnywhere, Category = "Identity") float attritionMultiplier = 1.f;
	UPROPERTY(EditAnywhere, Category = "Identity") int defenderBonus = 0;
	UPROPERTY(EditAnywhere, Category = "Identity") int vision = 0;
#pragma endregion

	TArray<AActor*> GetObjectsInHex();

	void CheckForHostility(AMovementAI* refTroop);
	void CheckForHostility(ABuilding* refBuilding);
	void AddTroopToHex(AMovementAI* troop);
	void AddBuildingToHex(ABuilding* newBuilding);
	void RemoveTroopFromHex(AMovementAI* troop);
	int GetNumberOfWorkers();
	float GetOutputPercent();

	void BeginBattle(AMovementAI* attacker = nullptr);
	Factions GetAttackerFaction();
	bool ActiveBattleOnHex();

	TMap<WorkerType, int> workersInHex;
	int maxWorkers = 15;

	float currentHarvestTime;
	float maxHarvestTime = 2.f;
	bool harvesting;
	float outputPercent;
	bool hasBuilding;
	UPROPERTY(VisibleAnywhere, Category = "Debug") bool attachmentCanBeVisible;

	UPROPERTY(EditAnywhere, Category = "Debug") bool debug = false;

	bool ActiveHarvesting();

	void UpdateResourceYield(StratResources resource, int value, Factions faction = Factions::None);
	void ToggleResourceYield();

	bool HasBuilding();

	FHexDisplay GetDisplayInfo();

	ABaseHex* FindFreeAdjacentHex(Factions faction, TArray<ABaseHex*> ignoredHexes);

	void SetFaction(Factions faction);

	void SetToTargetVolume(float& DeltaTime);
	UPROPERTY(EditAnywhere, Category = "Sound") float volumeSpeed = 1.5f;
	UPROPERTY(VisibleAnywhere, Category = "Sound") float targetVolume = 0.f;
	bool inSoundboxRadius = false;

	TerrainType GetHexTerrain();
	void SetHexTerrain(int maxSeedSize = 5, int randToMaintain = 5);
	void SetHexTerrain(TerrainType terrain);
	void SetHexModel();

	bool IsStaticBuildingTerrain();
	bool IsTraversableTerrain();
	bool IsBuildableTerrain();
	bool IsPlayerHex();
	bool CanPutWorkersOnHex();

private:
	AMovementAI* attackingTroop;
	TerrainType hexTerrain = TerrainType::Plains;
	int seedIndex = 0;

	TSet<ABaseHex*> GetSurroundingHexes();
};
