// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ManagerSpawner.generated.h"

UCLASS(HideCategories = (Rendering, Replication, Input, Actor, LOD, HLOD, Collision, Physics, Cooking, Events, WorldPartition, Events, DataLayers))
class CAPSTONEPROJECT_API AManagerSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AManagerSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION() void SpawnManagers();
	UPROPERTY(EditAnywhere, Category = "Managers") TArray<TSubclassOf<class AActor>> managers;
};
