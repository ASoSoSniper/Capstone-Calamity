// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementAI.h"
#include "Troop.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ATroop : public AMovementAI
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(VisibleAnywhere) bool attacking;

	virtual void CreatePath() override;
};
