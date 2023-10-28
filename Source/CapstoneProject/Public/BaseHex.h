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
	UPROPERTY(VisibleAnywhere) UInteractable* interactable;
	UPROPERTY(EditAnywhere) USceneComponent* troopAnchor;
	UPROPERTY(EditAnywhere) USceneComponent* buildingAnchor;

	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* hexMesh;
	UPROPERTY(EditAnywhere) USphereComponent* collider;

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

	TMap<StratResources, int> resourceBonuses;
	UPROPERTY(EditAnywhere) int energyYieldBonus = 1;
	UPROPERTY(EditAnywhere) int productionYieldBonus = 1;
	UPROPERTY(EditAnywhere) int foodYieldBonus = 1;
	UPROPERTY(EditAnywhere) int hexID;
	UPROPERTY(EditAnywhere) TerrainType hexTerrain;

#pragma endregion

	TArray<AActor*> GetObjectsInHex();

	void CheckForHostility(AMovementAI* refTroop);
	void AddTroopToHex(AMovementAI* troop);
	void RemoveTroopFromHex(AMovementAI* troop);

	void BeginBattle();

	AGlobalSpawner* spawner;

	bool battleInProgress;

	UPROPERTY(EditAnywhere) Factions hexOwner = Factions::None;

	TMap<WorkerType, int> workersInHex;

	float currentHarvestTime;
	float maxHarvestTime = 2.f;
	bool harvesting;

	void Harvest(float& DeltaTime);
	bool ActiveHarvesting();
};
