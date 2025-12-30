// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UnitActions.h"
#include "UAI_Building_QueueTroops.generated.h"

UENUM(BlueprintType)
enum class EBuildOrder : uint8
{
	Random,
	Price,
	Strength,
	BuildSpeed
};

UCLASS()
class CAPSTONEPROJECT_API UUAI_Building_QueueTroops : public UAI_Action
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;

private:
	UPROPERTY(EditAnywhere, Category = "Troops") TMap<UnitTypes, int> troopsToBuild;
	UPROPERTY(EditAnywhere, Category = "Troops") EBuildOrder buildOrder = EBuildOrder::Random;
	UPROPERTY(EditAnywhere, Category = "Troops") bool ascendingOrder = false;
	UPROPERTY(EditAnywhere, Category = "Troops") bool cycleTroopsQueued = false;
	UPROPERTY(EditAnywhere, Category = "Troops") int cycleTroopAtCount = 2;

	bool SortTroopTypes(const UnitTypes& a, const UnitTypes& b);
};
