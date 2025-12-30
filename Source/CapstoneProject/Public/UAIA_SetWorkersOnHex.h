// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "StratResources.h"
#include "UAIA_SetWorkersOnHex.generated.h"

UENUM(BlueprintType)
enum class EWorkersToSet : uint8
{
	Fill,
	Empty,
	SetAmount
};
UCLASS()
class CAPSTONEPROJECT_API UUAIA_SetWorkersOnHex : public UAI_Action
{
	GENERATED_BODY()

public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;

private:
	UPROPERTY(EditAnywhere, Category = "Workers") EStratResources resourceTarget;
	UPROPERTY(EditAnywhere, Category = "Workers") EWorkersToSet workerCount;
	UPROPERTY(EditAnywhere, Category = "Workers") int setAmount = 5;
};
