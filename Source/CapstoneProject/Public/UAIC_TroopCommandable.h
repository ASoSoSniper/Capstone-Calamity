// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "UAIC_TroopCommandable.generated.h"

class UAI_TroopCondition;

UCLASS()
class CAPSTONEPROJECT_API UUAIC_TroopCommandable : public UAI_Condition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Conditions") UAI_Condition* minSuccessCondition;
	UPROPERTY(EditAnywhere, Category = "Conditions") float staticMinSuccess = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Conditions") TArray<UAI_TroopCondition*> commandableConditions;
};
