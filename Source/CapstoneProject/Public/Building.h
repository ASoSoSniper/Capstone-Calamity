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
#include "Building.generated.h"


UCLASS()
class CAPSTONEPROJECT_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)UInteractable* interactable;
	UPROPERTY(VisibleAnywhere)UHexNav* hexNav;
	UPROPERTY(VisibleAnywhere)UUnitStats* unitStats;
	UPROPERTY(VisibleAnywhere)UMeshVisibility* visibility;
	UPROPERTY(EditAnywhere)UStaticMeshComponent* mesh;
	UPROPERTY(EditAnywhere)SpawnableBuildings buildingType;

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

	UPROPERTY(EditAnywhere) float buildTime = 5.f;
	UPROPERTY(EditAnywhere) float upgradeTime = 10.f;
	float currBuildTime;

	void Constructing(float& DeltaTime);
	void SetBuildState();
	virtual void Harvest(ABaseHex* hex);
	virtual void UpdateResources();
	virtual void RevertResources();
	virtual void BuildingAction();

	bool SphereCheck();
	UPROPERTY(EditAnywhere) float hexSearchDistance = 100.f;
	UPROPERTY(EditAnywhere) float hexSnapDistance = 50.f;

	UPROPERTY(VisibleAnywhere) AGlobalSpawner* spawner;

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
	virtual void DestroyingBuilding(float& DeltaTime);
	float destructionTime = 10.f;
	float currDestructionTime = 0.f;
	virtual void Destroyed() override;
};
