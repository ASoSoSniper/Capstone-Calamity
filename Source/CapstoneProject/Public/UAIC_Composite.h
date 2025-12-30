// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "UAIC_Composite.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIC_Composite : public UAI_Condition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Conditions") TArray<UAI_Condition*> conditions;
	UPROPERTY(EditAnywhere, Category = "Conditions") bool allMustBeTrue = true;
	UPROPERTY(EditAnywhere, Category = "Conditions") float minScoreForSuccess = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Conditions") float scoreOnSuccess = 1.f;
};
