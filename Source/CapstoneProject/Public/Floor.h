// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Floor.generated.h"

UCLASS()
class CAPSTONEPROJECT_API AFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) UMaterialInterface* material;
	UPROPERTY(EditAnywhere) UMaterialInstanceDynamic* floorMaterial;
	UPROPERTY(EditAnywhere) UStaticMeshComponent* mesh;
	UPROPERTY(EditAnywhere) UInteractable* interactable;
	UPROPERTY(EditAnywhere) float worldTime;
	UPROPERTY(EditAnywhere) bool disableSpeedChange;
};
