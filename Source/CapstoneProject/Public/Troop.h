// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementAI.h"
#include "GlobalSpawner.h"
#include "AITroopComponent.h"
#include "Troop.generated.h"

/**
 * 
 */
class AMergedArmy;
class UAITroopComponent;

UCLASS()
class CAPSTONEPROJECT_API ATroop : public AMovementAI
{
	GENERATED_BODY()
	
public:
	ATroop();
	UPROPERTY(EditAnywhere) UAITroopComponent* AITroopComponent;
	AActor* targetToMerge;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void MergeOnTile();
	
	virtual void Action1();
	virtual void Action2();
	virtual void Action3();
	virtual void Action4();

	UFUNCTION(BlueprintCallable) FTroopUIData GetUIData();
	bool CommandTroopToMerge(AActor* target);
	int GetArmyCap();

protected:
	virtual void MoveToTarget(float& DeltaTime) override;
	virtual void CreatePath() override;

private:
	bool SetUpTroop();
	bool setupComplete;
	void RotateToFaceTarget(FVector direction, float& DeltaTime);

	UPROPERTY(EditAnywhere) float rotateSpeedMultiplier = 3.f;

	UPROPERTY(EditAnywhere) bool EndForVSlice = false;
	UPROPERTY(EditAnywhere) bool debug = false;
	UPROPERTY(EditAnywhere) int armyCap = 20;
	UPROPERTY(VisibleAnywhere) bool merging;
};
