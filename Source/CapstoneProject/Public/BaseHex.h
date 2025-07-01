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

USTRUCT(BlueprintType)
struct FCurrentResourceYields
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int foodYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealthYield;
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

	UPROPERTY(VisibleAnywhere) ABuilding* building;

	UPROPERTY(VisibleAnywhere) ABattleObject* battle;

#pragma endregion

//Variables that change depending on the hex's identity
#pragma region Identity

	Factions GetHexOwner();
	void SetHexOwner(Factions faction);

	int GetMovementMulti() const;
	int GetAttritionMulti() const;
	int GetDefenderBonus() const;
	int GetVision() const;
#pragma endregion

	TArray<AActor*> GetObjectsInHex() const;

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
	int GetMaxWorkers();
	void SetMaxWorkers(int newMax);
	UPROPERTY(VisibleAnywhere) TArray<AMovementAI*> troopsInHex;

	bool ActiveHarvesting();

	void UpdateResourceYield(StratResources resource, int value, Factions faction = Factions::None);
	void ToggleResourceYield();

	ABuilding* GetBuilding();
	void SetBuilding(ABuilding* setBuilding);

	FHexDisplay GetDisplayInfo();

	ABaseHex* FindFreeAdjacentHex(Factions faction, TArray<ABaseHex*> ignoredHexes);

	float GetTargetVolume();
	void SetTargetVolume(float volume);
	void SetInSoundBoxRadius(bool inRadius);

	TerrainType GetHexTerrain();
	TSet<ABaseHex*> GetSurroundingHexes();
	void SetHexTerrain(int maxSeedSize = 5, int randToMaintain = 5);
	void SetHexTerrain(TerrainType terrain);
	void SetHexModel();
	void SetAttachmentCanBeVisible(bool canBeVisible);

	bool IsStaticBuildingTerrain();
	bool IsTraversableTerrain() const;
	bool IsBuildableTerrain();
	bool IsPlayerHex();
	bool CanPutWorkersOnHex();

	FCurrentResourceYields GetCurrentResourceYields();

private:

	AMovementAI* attackingTroop;
	TerrainType hexTerrain = TerrainType::Plains;
	int seedIndex = 0;
	bool harvesting;
	float outputPercent;

	TMap<StratResources, int> resourceBonuses;

	UPROPERTY(EditAnywhere, Category = "Identity") Factions hexOwner = Factions::None;
	UPROPERTY(EditAnywhere, Category = "Identity") TMap<TerrainType, FHexInfo> hexInfo;
	UPROPERTY(VisibleAnywhere, Category = "Identity") float moveMultiplier = 1.f;
	UPROPERTY(VisibleAnywhere, Category = "Identity") float attritionMultiplier = 1.f;
	UPROPERTY(VisibleAnywhere, Category = "Identity") int defenderBonus = 0;
	UPROPERTY(VisibleAnywhere, Category = "Identity") int vision = 0;
	UPROPERTY(VisibleAnywhere, Category = "Identity") int maxWorkers = 15;
	UPROPERTY(VisibleAnywhere, Category = "Identity") int maxWorkersDefault = 15;

	UPROPERTY(EditAnywhere, Category = "Sound") float volumeSpeed = 1.5f;
	UPROPERTY(VisibleAnywhere, Category = "Sound") float targetVolume = 0.f;
	void SetToTargetVolume(float& DeltaTime);
	bool inSoundboxRadius = false;

	UPROPERTY(VisibleAnywhere, Category = "Debug") bool attachmentCanBeVisible;

	UPROPERTY(EditAnywhere, Category = "Debug") bool debug = false;
};
