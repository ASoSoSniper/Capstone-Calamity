// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UnitActions.h"
#include "UAIA_Troop_SplitArmy.generated.h"

UENUM(BlueprintType)
enum class ESplitType : uint8
{
	InHalf,
	OneUnit
};

UCLASS()
class CAPSTONEPROJECT_API UUAIA_Troop_SplitArmy : public UAI_Action
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;

private:
	UPROPERTY(EditAnywhere, Category = "Split Rules") ESplitType splitType;
	UPROPERTY(EditAnywhere, Category = "Split Rules") UnitTypes unitToSplit;
};
