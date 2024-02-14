// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "PowerPlant.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API APowerPlant : public ABuilding
{
	GENERATED_BODY()
public:
	APowerPlant();
	
	virtual void UpdateResources() override;
	virtual void RevertResources() override;

	UPROPERTY(EditAnywhere) int energyYield = 2;
};
