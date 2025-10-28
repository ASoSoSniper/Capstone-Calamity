// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "BaseHex.h"
#include "GlobalSpawner.h"
#include "Outpost.generated.h"

/**
 * 
 */

UCLASS()
class CAPSTONEPROJECT_API AOutpost : public ABuilding
{
	GENERATED_BODY()

public:

	AOutpost();

	virtual void Tick(float DeltaTime) override;

	virtual void Action1() override;
	virtual void Action2() override;
	virtual void Action3() override;
	virtual void Action4() override;

	virtual void Destroyed() override;
protected:

	UPROPERTY(EditAnywhere) int range = 2;

	UPROPERTY(EditAnywhere)
		float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere)
		float traceLength = 50.f;
	UPROPERTY(VisibleAnywhere) TSet<ABaseHex*> claimedHexes;

	UStaticMesh* LoadFinishedModel() override;
	virtual void SetToFinishedModel() override;

private:
	TSet<ABaseHex*> ClaimLand();

	void BuildingAction() override;
};
