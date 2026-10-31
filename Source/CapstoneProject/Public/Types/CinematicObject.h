// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CinematicObject.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FCinematicObject
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) AActor* object;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float travelTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float lingerTime;
};
