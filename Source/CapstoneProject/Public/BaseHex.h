// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexInfo.h"
#include "Interactable.h"
#include "Components/SphereComponent.h"
#include "Battle.h"
#include "BaseHex.generated.h"

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

	UPROPERTY(VisibleAnywhere) UHexInfo* hexInfo;
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

	UPROPERTY(VisibleAnywhere) AActor* building;

	UPROPERTY(VisibleAnywhere) TArray<AActor*> troopsInHex;
	TArray<Battle*> battlesInHex;

#pragma endregion

	TArray<AActor*> GetObjectsInHex();

	bool CheckForHostility();
};
