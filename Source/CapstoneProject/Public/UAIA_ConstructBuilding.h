// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UnitActions.h"
#include "UAIA_ConstructBuilding.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIA_ConstructBuilding : public UAI_Action
{
	GENERATED_BODY()

public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;

private:
	UPROPERTY(EditAnywhere, Category = "Building") SpawnableBuildings building;
};
