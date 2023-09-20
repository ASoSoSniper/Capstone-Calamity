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

	virtual void MergeOnTile();

	virtual void Action1();
	virtual void Action2();
	virtual void Action3();
	virtual void Action4();
};
