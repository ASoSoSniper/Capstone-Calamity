// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outpost.h"
#include "CapitolHub.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ACapitolHub : public AOutpost
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
	virtual void UpdateResources() override;

	UPROPERTY(EditAnywhere) int foodYield = 1;
	UPROPERTY(EditAnywhere) int productionYield = 1;
	UPROPERTY(EditAnywhere) int energyYield = 1;
};
