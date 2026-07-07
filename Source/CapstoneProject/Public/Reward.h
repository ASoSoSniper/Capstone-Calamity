// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Reward.generated.h"

class UFaction;

UCLASS()
class CAPSTONEPROJECT_API UReward : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void ApplyReward(UFaction* faction);
	UFUNCTION(BlueprintCallable, BlueprintPure) virtual FString GetRewardText() const;
};
