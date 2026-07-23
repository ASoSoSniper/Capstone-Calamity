// Fill out your copyright notice in the Description page of Project Settings.


#include "Reward.h"
#include "Faction.h"

void UReward::ApplyReward(UFaction* faction)
{
}

FString UReward::GetRewardText() const
{
    return TEXT("Placeholder text");
}

bool UReward::HiddenReward() const
{
    return hiddenInWorldEvent;
}
