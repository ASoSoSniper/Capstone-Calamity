// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "MiningStation.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API AMiningStation : public ABuilding
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void Mine(float& DeltaTime);

	UPROPERTY(EditAnywhere) float mineTime = 2.f;
	float currMineTime;
};
