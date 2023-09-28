// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementAI.h"
#include "Troop.generated.h"

/**
 * 
 */
class AMergedArmy;
UCLASS()
class CAPSTONEPROJECT_API ATroop : public AMovementAI
{
	GENERATED_BODY()
	
public:
	ATroop();
	UPROPERTY(VisibleAnywhere) bool attacking;
	UPROPERTY(VisibleAnywhere) bool merging;
	ATroop* targetToMerge;
	virtual void Tick(float DeltaTime) override;
	virtual void CreatePath() override;
	virtual void MergeOnTile();
	
	void MergeArmies(ATroop* seeker, ATroop* target);
	void InputUnitStats(UnitActions::UnitData data);

	virtual void Action1();
	virtual void Action2();
	virtual void Action3();
	virtual void Action4();

	TSubclassOf<class AMergedArmy> mergedArmyPrefab;
	TSubclassOf<class ATroop> troopPrefab;
};
