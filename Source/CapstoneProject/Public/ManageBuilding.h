// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageMode.h"
#include "Building.h"
#include "ManageBuilding.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UManageBuilding : public UManageMode
{
	GENERATED_BODY()
public:	
	ABuilding* selectedBuilding;

	virtual void Select(AActor* selectedObject) override;
	virtual void SwitchState() override;
	virtual void CheckSelection() override;
};
