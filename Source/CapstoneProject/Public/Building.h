// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "HexNav.h"
#include "UnitStats.h"
#include "Components/BoxComponent.h"
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
	UPROPERTY(EditAnywhere)UBoxComponent* collider;
	UPROPERTY(EditAnywhere)UStaticMeshComponent* mesh;

	enum BuildStates
	{
		None,
		Building,
		Complete,
		Uprading,
		Upgraded
	};
	BuildStates buildState = None;

	UPROPERTY(EditAnywhere) float buildTime = 5.f;
	UPROPERTY(EditAnywhere) float upgradeTime = 10.f;
	float currBuildTime;

	void Constructing(float& DeltaTime);
	void SetBuildState();
	virtual void Harvest(ABaseHex* hex);
	virtual void UpdateResources();
	virtual void BuildingAction();

	void SphereCheck();
	UPROPERTY(EditAnywhere) float hexSnapDistance = 20.f;
};
