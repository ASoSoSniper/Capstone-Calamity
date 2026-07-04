// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objective.h"
#include "ObjectiveMulti.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UObjectiveMulti : public UObjective
{
	GENERATED_BODY()
	
public:
	virtual FString GetObjectiveDisplay() const override final;
	virtual void SetupObjective(UFaction* faction) override final;
protected:
	virtual void CleanupObjective() override final;
	void HandleOnCompleted(UObjective* subObjective);
private:
	UPROPERTY(EditAnywhere, Category = "Objectives") TArray<UObjective*> subObjectives;
	UPROPERTY(EditAnywhere, Category = "Objectives") bool completeInOrder = false;

	TMap<UObjective*, bool> complete;
	int index = 0;
};
