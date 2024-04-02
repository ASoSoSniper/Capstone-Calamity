// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "UnitActions.h"
#include "MergedArmy.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API AMergedArmy : public ATroop
{
	GENERATED_BODY()

public:
	AMergedArmy();

	void ConsumeUnit(ATroop* mergedUnit);
	void ConsumeArmy(AMergedArmy* mergedArmy);
	void ConsumeData(TArray<UnitActions::UnitData>& groupData);
	ATroop* SpawnUnit(TArray<UnitActions::UnitData>& groupData);

	void SplitInHalf();
	virtual void Action1() override;
	virtual void Action3() override;
	virtual void Action4() override;
	
};
