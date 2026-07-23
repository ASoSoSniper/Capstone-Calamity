// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Reward.h"
#include "Rwrd_EventScheduled.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API URwrd_EventScheduled : public UReward
{
	GENERATED_BODY()
	
public:
	virtual void ApplyReward(UFaction* faction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Reward") int daysAhead = 1;
	UPROPERTY(EditAnywhere, Category = "Reward") FString eventKey;
};
