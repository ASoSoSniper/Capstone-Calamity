// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "MaterialStorage.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API AMaterialStorage : public ABuilding
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere) int storageIncrease = 10;

	virtual void UpdateResources() override;
};
