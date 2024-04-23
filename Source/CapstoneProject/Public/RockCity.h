// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlienCity.h"
#include "RockCity.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ARockCity : public AAlienCity
{
	GENERATED_BODY()
	

public:
	ARockCity();
	virtual void SetToFinishedModel() override;
	virtual bool IsDisabled() override;

	UCinematicPosition* cinematicComponent;
};
