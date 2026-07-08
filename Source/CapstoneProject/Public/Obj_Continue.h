// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objective.h"
#include "Obj_Continue.generated.h"

struct FDateTickUpdate;

UCLASS()
class CAPSTONEPROJECT_API UObj_Continue : public UObjective
{
	GENERATED_BODY()
	
public:
	virtual void SetupObjective(UFaction* faction) override;

protected:
	virtual void CleanupObjective() override;
	void HandleWorldTick(const FDateTickUpdate& update);

private:
	UPROPERTY(EditAnywhere, Category = "Delay") int ticksToComplete = 0;
	int currentTicks = 0;
};
