// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "StratResources.h"
#include "UnitActions.h"
#include "UAIC_PriorityHexesAvailable.generated.h"

/**
 * Check hexes assigned to resources to determine whether valid results are recieved. Boolean condition.
 * 0 = no spaces available to place workers.
 * 1 = all priority hex calls return available spaces to place workers.
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_PriorityHexesAvailable : public UAI_Condition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Resources") TMap<EStratResources, SpawnableBuildings> resources;
};
