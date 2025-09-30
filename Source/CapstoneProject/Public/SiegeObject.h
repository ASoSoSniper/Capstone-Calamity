// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleObject.h"
#include "SiegeObject.generated.h"

/**
 * 
 */

class ABasePlayerController;
UCLASS(BlueprintType)
class CAPSTONEPROJECT_API ASiegeObject : public ABattleObject
{
	GENERATED_BODY()
	
public:
	virtual void Start() override;

	ABuilding* GetBuilding();

protected:
	virtual void CalculateGroupDamage(int& group1Damage, int& group2Damage) override;
	virtual void ApplyGroupDamage(const int& group1Damage, const int& group2Damage) override;

	virtual void GenerateModels() override;
	virtual void DestroyBattle() override;

	ABuilding* building;
	int group1SiegeDamage;

	bool BuildingIsAlive() const;
	virtual bool EndCondition() const override;
	virtual bool Group2IsAlive(bool& containsHuman) const override;
};
