// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Farmland.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API AFarmland : public ABuilding
{
	GENERATED_BODY()
public:
	AFarmland();

	virtual void UpdateResources() override;
	virtual void RevertResources() override;
	virtual UStaticMesh* LoadFinishedModel() override;
	UFUNCTION(BlueprintCallable) bool ToggleResourcesProduced(bool produceFood);

	bool producingFood = true;
};
