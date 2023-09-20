// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageMode.h"
#include "MovementAI.h"
#include "ManageTroop.generated.h"

/**
 * 
 */

UCLASS()
class CAPSTONEPROJECT_API UManageTroop : public UManageMode
{
	GENERATED_BODY()
	
public:
	AMovementAI* selectedTroop;
	virtual void Select(AActor* selectedObject) override;
	virtual void SwitchState(UnitActions::SelectionIdentity& info) override;

};
