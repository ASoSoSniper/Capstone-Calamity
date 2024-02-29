// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingAttachment.h"
#include "OutpostDefenses.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UOutpostDefenses : public UBuildingAttachment
{
	GENERATED_BODY()

public:
	UOutpostDefenses();
	virtual void UpdateResources() override;
	virtual void DisableAttachment() override;

	UPROPERTY(EditAnywhere) int HPInrease = 20;
	UPROPERTY(EditAnywhere) int siegeIncrease = 10;
};
