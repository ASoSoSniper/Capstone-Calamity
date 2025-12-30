// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "StratResources.h"
#include "UAIC_ResourceCompare.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_ResourceCompare : public UAI_Condition
{
	GENERATED_BODY()
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	//The resource to check.
	UPROPERTY(EditAnywhere, Category = "Resource") EStratResources resource;
	//Determines whether to check if this resource is being over or underproduced,
	//compared to the second best/worst-performing resource.
	UPROPERTY(EditAnywhere, Category = "Resource") bool greatestCount = true;
	//Determines whether buildings in construction, and thus not yet producing resources,
	//should factor into the resource comparison.
	UPROPERTY(EditAnywhere, Category = "Resource") bool includeIncompleteBuildings = false;
	//Determines whether to skip wealth in this comparison, due to its unique form of aquisition.
	UPROPERTY(EditAnywhere, Category = "Resource") bool includeWealth = false;
};
