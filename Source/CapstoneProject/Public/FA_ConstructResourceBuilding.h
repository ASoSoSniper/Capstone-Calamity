// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionAction.h"

/**
 * 
 */
class CAPSTONEPROJECT_API FA_ConstructResourceBuilding : public FactionAction
{
public:
	FA_ConstructResourceBuilding();
	~FA_ConstructResourceBuilding();

	virtual int ScoreAction(Faction_AIController& controller) override;
	virtual void PerformAction(Faction_AIController& controller) override;

private:
	float lowRatio = 0.5f;
	SpawnableBuildings GetNeededBuilding(Faction_AIController& controller);
};
