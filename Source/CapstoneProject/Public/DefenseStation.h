// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "DefenseStation.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ADefenseStation : public ABuilding
{
	GENERATED_BODY()
	
public:
	ADefenseStation();

protected:
	virtual void BuildingAction() override;
	virtual void Destroyed() override;

private:
	void ClaimLand();

	UPROPERTY(EditAnywhere, Category = "Buffs") int buffRadius = 2;
	UPROPERTY(EditAnywhere, Category = "Buffs") int moraleBuff = 5;
	UPROPERTY(EditAnywhere, Category = "Buffs") int damageBuff = 5;
	UPROPERTY(EditAnywhere, Category = "Buffs") int siegeDamageBuff = 5;

	FStatusEffect* buff;
	TSet<ABaseHex*> buffedHexes;
};
