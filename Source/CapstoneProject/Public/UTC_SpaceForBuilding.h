// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_TroopCondition.h"
#include "UnitActions.h"
#include "UTC_SpaceForBuilding.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUTC_SpaceForBuilding : public UAI_TroopCondition
{
	GENERATED_BODY()
public:
	virtual float ScoreCondition(UFaction* faction, ABaseHex* hex) const override;
private:
	UPROPERTY(EditAnywhere, Category = "Building") SpawnableBuildings building;
};
