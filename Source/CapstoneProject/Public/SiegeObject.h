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
	virtual void Attack() override;
	virtual void EndBattle() override;

	virtual void GenerateModels() override;
	virtual void DestroyBattle() override;

	ABuilding* building;

	int attackerSiegeDamage;
	void CalculateSiegeDamage();
	bool BuildingIsAlive();
};
