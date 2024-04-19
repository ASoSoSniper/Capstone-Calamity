// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ExploreAnims.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UExploreAnims : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Animation") bool isWalking;
	UPROPERTY(BlueprintReadWrite, Category = "Animation") float timeScale;
};
