// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexInfo.h"
#include "Interactable.h"
#include "Components/SphereComponent.h"
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

	UPROPERTY(VisibleAnywhere) UHexInfo* hexInfo;
	UPROPERTY(VisibleAnywhere) UInteractable* interactable;
	UPROPERTY(EditAnywhere) USceneComponent* troopAnchor;
	UPROPERTY(EditAnywhere) USceneComponent* buildingAnchor;

	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* hexMesh;
	UPROPERTY(EditAnywhere) USphereComponent* collider;

	UPROPERTY(EditAnywhere) bool isSelected = false;
	UPROPERTY(EditAnywhere) bool hasFOW = true;

	UPROPERTY(VisibleAnywhere) TArray<AActor*> troopsInHex;
	UPROPERTY(VisibleAnywhere) TArray<AActor*> battlesInHex;
};
