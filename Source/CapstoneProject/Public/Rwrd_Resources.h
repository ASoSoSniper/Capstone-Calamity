// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Reward.h"
#include "StratResources.h"
#include "Rwrd_Resources.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API URwrd_Resources : public UReward
{
	GENERATED_BODY()

public:
	virtual void ApplyReward(UFaction* faction) override;
	virtual FString GetRewardText() const override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Rewards") TMap<EStratResources, int32> resources;
};
