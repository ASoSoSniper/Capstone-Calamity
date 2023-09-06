// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TerrainEnum.h"
#include "StratResources.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HexInfo.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UHexInfo : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere) int hexID;
	UPROPERTY(EditAnywhere) int foodVal;
	UPROPERTY(EditAnywhere) int prodVal;
	UPROPERTY(EditAnywhere) TerrainType hexTerrain;
	UPROPERTY(EditAnywhere) StratResources hexStratResources;


public:	
	// Sets default values for this component's properties
	UHexInfo();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
