// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_HexCondition.h"
#include "UHC_AvailableRadius.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUHC_AvailableRadius : public UAI_HexCondition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(ABaseHex* hex) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Area") int radius = 0;
};
