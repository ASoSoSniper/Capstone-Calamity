// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StratResources.h"
#include "PointOfInterest.generated.h"

USTRUCT(BlueprintType)
struct FPointOfInterest
{
	GENERATED_USTRUCT_BODY()

private:
	UPROPERTY(EditAnywhere) FString pointTitle = TEXT("Hidden Cache");
	UPROPERTY(EditAnywhere) TMap<EStratResources, int32> rewards;
	UPROPERTY(EditAnywhere) int32 daysToComplete = 1;
	int32 daysRemaining = 0;
	int32 hoursRemaining = 0;
	int32 minutes = 0;
public:
	FPointOfInterest() {};
	FPointOfInterest(FPointOfInterest& setPOI)
	{
		pointTitle = setPOI.pointTitle;
		rewards = setPOI.rewards;
		daysToComplete = setPOI.daysToComplete;
		daysRemaining = setPOI.daysToComplete;
	}
	FPointOfInterest(TMap<EStratResources, int32> setRewards, FString setTitle = TEXT("Hidden Cache"), int32 setDays = 1) :
		pointTitle(setTitle), rewards(setRewards), daysToComplete(setDays), daysRemaining(setDays) {
	}

	FString GetPointTitle() const;
	const TMap<EStratResources, int32>& GetRewards() const;
	FString GetWorldDisplay() const;
	int32 GetDaysToComplete() const;
	bool Work();
	bool WorkCompleted() const;
	float GetWorkProgress() const;
};