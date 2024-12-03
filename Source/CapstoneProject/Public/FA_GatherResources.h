// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionAction.h"
#include "StratResources.h"

/**
 * 
 */
class CAPSTONEPROJECT_API FA_GatherResources : public FactionAction
{
public:
	FA_GatherResources();
	~FA_GatherResources();

	virtual int ScoreAction(Faction_AIController& controller) override;
	virtual void PerformAction(Faction_AIController& controller) override;

private:
	int LowResources(Faction_AIController& controller, StratResources resource);

	float lowResourcePercent = 0.25f;
};
