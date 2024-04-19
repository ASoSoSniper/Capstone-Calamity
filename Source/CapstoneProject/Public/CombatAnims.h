// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CombatAnims.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UCombatAnims : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Animation") bool isFightWon;
	UPROPERTY(BlueprintReadWrite, Category = "Animation") bool isFightLost;
	UPROPERTY(BlueprintReadWrite, Category = "Animation") float timeScale;
};
