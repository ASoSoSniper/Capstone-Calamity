// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionAction.h"

/**
 * 
 */
class CAPSTONEPROJECT_API FA_ConstructOutpost : public FactionAction
{
public:
	FA_ConstructOutpost();
	~FA_ConstructOutpost();

	virtual int ScoreAction(Faction_AIController& controller) override;
	virtual void PerformAction(Faction_AIController& controller) override;
};
