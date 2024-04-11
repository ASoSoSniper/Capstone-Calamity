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

	virtual bool SetUpAttachment(BuildingAttachments attachment) override;

	UPROPERTY(EditAnywhere) int HPIncrease = 20;
	UPROPERTY(EditAnywhere) int siegeIncrease = 10;
};
