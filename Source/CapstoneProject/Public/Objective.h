// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Objective.generated.h"

class UFaction;
class UReward;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveComplete, UObjective*, objective);

UCLASS()
class CAPSTONEPROJECT_API UObjective : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) FString GetObjectiveTitle() const;
	UFUNCTION(BlueprintCallable) FString GetObjectiveDescription() const;
	UFUNCTION(BlueprintCallable) const TArray<UReward*>& GetRewards() const;

	virtual void SetupObjective(UFaction* faction);

	FOnObjectiveComplete onObjectiveComplete;
	
protected:
	void CompleteObjective();
	virtual void CleanupObjective();
	//virtual void HandleUpdate(payload);
	//int settlersBuilt = 0;
private:
	UPROPERTY(EditAnywhere, Category = "Display") FString title;
	UPROPERTY(EditAnywhere, Category = "Display") FString description;
	UPROPERTY(EditAnywhere, Category = "Rewards") TArray<UReward*> rewards;

	UFaction* factionObject;
};
