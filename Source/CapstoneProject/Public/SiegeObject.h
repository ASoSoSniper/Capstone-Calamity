// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleObject.h"
#include "SiegeObject.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ASiegeObject : public ABattleObject
{
	GENERATED_BODY()
	
public:
	virtual void Start() override;
	virtual void Attack() override;
	virtual void EndBattle() override;

	ABuilding* building;

	int attackerSiegeDamage;
	void CalculateSiegeDamage();
	bool BuildingIsAlive();
};
