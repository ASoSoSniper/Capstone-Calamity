// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "HexNav.h"
#include "UnitStats.h"
#include "MeshVisibility.h"
#include "Components/BoxComponent.h"
#include "GlobalSpawner.h"
#include "CinematicPosition.h"
#include "UAI_Controller.h"
#include "Building.generated.h"


UCLASS()
class CAPSTONEPROJECT_API ABuilding : public AActor, public IUAI_Controller
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere) UCinematicPosition* cinematicComponent;
	UPROPERTY(VisibleAnywhere)UInteractable* interactable;
	UPROPERTY(EditAnywhere)UStaticMeshComponent* mesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere)UHexNav* hexNav;
	UPROPERTY(VisibleAnywhere)UUnitStats* unitStats;
	UPROPERTY(VisibleAnywhere)UMeshVisibility* visibility;

	virtual void Action1();
	virtual void Action2();
	virtual void Action3();
	virtual void Action4();
	virtual void Action5();
	virtual void Action6();
	virtual void Action7();
	virtual void Action8();
	virtual void Action9();
	virtual void Action10();
	
	virtual void BeginDestroying();
	virtual bool IsDisabled();

	bool SetSiegeState(bool sieging, Factions occupier = Factions::None);
	bool IsOccupied();
	void SetBuildAtStart(bool active);
	
	bool TroopOccupation();
	int GetOccupyingTroops();
	int GetOccupationMinCount();
	void HealOverTime();
	UFUNCTION(BlueprintCallable) Factions GetOccupier();
	UFUNCTION(BlueprintCallable) float GetBuildPercent();
	UFUNCTION(BlueprintCallable) float GetUnrestPercent();
	UFUNCTION(BlueprintCallable) SpawnableBuildings GetBuildingType();

protected:
	enum BuildStates
	{
		None,
		Building,
		Complete,
		Uprading,
		Upgraded,
		Destroying
	};
	BuildStates buildState = None;

	UPROPERTY(VisibleAnywhere) SpawnableBuildings buildingType;
	TMap<StratResources, int> resourceYields;
	int resourceCapIncrease = 0;
	UPROPERTY(EditAnywhere, Category = "Initialization") bool builtAtStart = false;

	virtual void UpdateResources();
	virtual void RevertResources();
	virtual void BuildingAction();
	virtual void SetToFinishedModel();
	virtual void Destroyed() override;
	virtual void DestroyingBuilding(float& DeltaTime);

private:
	bool SetupBuilding(SpawnableBuildings type);
	void Constructing(float& DeltaTime);
	void UpdateUnrestLevel(float& DeltaTime);
	bool SphereCheck();
	void SetBuildState();
	bool setupComplete;

	UPROPERTY(EditAnywhere, Category = "Occupation") int troopOccupationMin = 3;
	UPROPERTY(EditAnywhere, Category = "Occupation") float occupyResourcePercent = 0.25f;
	UPROPERTY(VisibleAnywhere, Category = "Occupation") bool occupied = false;
	UPROPERTY(VisibleAnywhere, Category = "Occupation") Factions occupyingFaction = Factions::None;
	UPROPERTY(VisibleAnywhere, Category = "Unrest") float unrestLevel = 0.f;
	UPROPERTY(EditAnywhere, Category = "Unrest") float unrestTime = 20.f;

	UPROPERTY(EditAnywhere, Category = "Healing") float healPercent = 0.04f;

	UPROPERTY(EditAnywhere, Category = "Initialization") float hexSearchDistance = 100.f;
	UPROPERTY(EditAnywhere, Category = "Initialization") float hexSnapDistance = 50.f;

	UPROPERTY(EditAnywhere, Category = "Initialization") float buildTime = 5.f;
	float currBuildTime;

	UPROPERTY() AActor* smokeEffect;

	UPROPERTY(EditAnywhere, Category = "Destruction") float destructionTime = 10.f;
	float currDestructionTime = 0.f;
};
