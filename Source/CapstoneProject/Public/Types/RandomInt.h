// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RandomInt.generated.h"

USTRUCT(BlueprintType)
struct FRandomInt {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int randomMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int randomMax;
};