// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingAttachment.h"
#include "OutpostStorage.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UOutpostStorage : public UBuildingAttachment
{
	GENERATED_BODY()
	
	virtual void UpdateResources() override;

public:
	UOutpostStorage();
	virtual void ActivateAttachment() override;
	virtual void DisableAttachment() override;

	UPROPERTY(EditAnywhere) int storageIncrease = 200;
};
