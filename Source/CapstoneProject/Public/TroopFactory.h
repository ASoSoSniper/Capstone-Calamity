// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "TroopFactory.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ATroopFactory : public ABuilding
{
	GENERATED_BODY()
	
public:
	ATroopFactory();
	virtual void Tick(float DeltaTime) override;

	void QueueTroopBuild(UnitTypes unit);
	UFUNCTION(BlueprintCallable, BlueprintPure) float GetTroopBuildTime() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) float GetTroopBuildAlpha() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) UTexture2D* GetQueuedTroopIcon() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) UnitTypes GetQueuedTroop() const;
	int GetQueueSize() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) bool BuildingTroop() const;
private:
	void BuildTroop(const float& DeltaTime);
	void SpawnTroop();

	void EnqueueTroop(UnitTypes unit);
	UnitTypes DequeueTroop();

	TQueue<UnitTypes> queuedUnits;
	int queueSize = 0;
	float currentTroopBuildTime = 0.f;
};
