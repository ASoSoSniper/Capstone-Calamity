// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Condition.h"
#include "StratResources.h"
#include "UAIC_ResourceCheck.generated.h"

USTRUCT(BlueprintType)
struct FResourceSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere) bool greaterEqual = true;
	UPROPERTY(EditAnywhere) int32 value = 100;
};
UCLASS()
class CAPSTONEPROJECT_API UUAIC_ResourceCheck : public UAI_Condition
{
	GENERATED_BODY()
	
public:
	virtual float ScoreCondition(IUAI_Controller* controller) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Resources") TMap<EStratResources, FResourceSetting> resourcesToCheck;
};
