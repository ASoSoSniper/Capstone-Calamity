// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StratResources.h"
#include "UnitActions.h"
#include "WorkerStats.generated.h"

USTRUCT(BlueprintType)
struct FWorkerStats
{
	GENERATED_USTRUCT_BODY()

public:
	int working;
	UPROPERTY(EditAnywhere) int available;
	int maxAcquired = 100;

	UPROPERTY(EditAnywhere) TMap<EStratResources, int> resourcePerAvailable;
	UPROPERTY(EditAnywhere) TMap<EStratResources, int> resourcePerWorking;
};