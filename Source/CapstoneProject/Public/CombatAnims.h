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
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool isFightWon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool isFightLost;
};
