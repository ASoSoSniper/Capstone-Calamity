// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Components/SphereComponent.h"
#include "TerrainEnum.h"
#include "StratResources.h"
#include "UnitActions.h"
#include "BaseHex.generated.h"

class AMergedArmy;
class AMovementAI;
class ABuilding;
class AGlobalSpawner;
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
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* troopAnchor;
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* buildingAnchor;

	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMeshAttachment;

	UPROPERTY(EditAnywhere, Category = "Components") USphereComponent* collider;

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
		int capBonus;
	};

	TMap<StratResources, ResourceStats> resourceBonuses;

	enum VisibilityStatus
	{
		Undiscovered,
		Discovered,
		Visible
	};
	struct Visibility
	{
		VisibilityStatus status = Undiscovered;
		bool inSight = false;
		bool discoveredByFaction = false;
	};

	UPROPERTY(EditAnywhere, Category = "Resources") int energyYieldBonus = 1;
	UPROPERTY(EditAnywhere, Category = "Resources") int productionYieldBonus = 1;
	UPROPERTY(EditAnywhere, Category = "Resources") int foodYieldBonus = 1;
	UPROPERTY(EditAnywhere, Category = "Identity") int hexID;
	UPROPERTY(EditAnywhere, Category = "Identity") TerrainType hexTerrain = TerrainType::Plains;
	TerrainType terrainChange = TerrainType::None;
	UPROPERTY(EditAnywhere, Category = "Identity") Factions hexOwner = Factions::None;
#pragma endregion

	TArray<AActor*> GetObjectsInHex();

	void CheckForHostility(AMovementAI* refTroop);
	void AddTroopToHex(AMovementAI* troop);
	void RemoveTroopFromHex(AMovementAI* troop);

	void BeginBattle();

	AGlobalSpawner* spawner;

	bool battleInProgress;

	TMap<WorkerType, int> workersInHex;
	TMap<Factions, Visibility> factionVisibility;

	float currentHarvestTime;
	float maxHarvestTime = 2.f;
	bool harvesting;

	void Harvest(float& DeltaTime);
	bool ActiveHarvesting();

	void UpdateFoodYield(int value);
	void UpdateProductionYield(int value);
	void UpdateEnergyYield(int value);
	void UpdateResourceCapIncrease(int value);

	void ToggleResourceYield();

	void ToggleVisibility(Factions faction);
	void SetVisibility();
	void RequestTerrainChange();

	UPROPERTY(VisibleAnywhere) int visibilityToHumans;

	
};
