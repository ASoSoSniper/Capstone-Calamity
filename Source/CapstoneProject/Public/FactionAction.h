// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Faction_AIController.h"

/**
 * 
 */
class CAPSTONEPROJECT_API FactionAction
{
public:
	FactionAction();
	~FactionAction();

	virtual int ScoreAction(Faction_AIController& controller) = 0;
	virtual void PerformAction(Faction_AIController& controller) = 0;
};
