// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Reward.h"
#include "Rwrd_Event.generated.h"

UCLASS()
class CAPSTONEPROJECT_API URwrd_Event : public UReward
{
	GENERATED_BODY()
	
public:
	virtual void ApplyReward(UFaction* faction) override;
	virtual FString GetRewardText() const override;
private:
	UPROPERTY(EditAnywhere, Category = "Reward") FString eventKey;
};
