// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitStats.h"

/**
 * 
 */
class CAPSTONEPROJECT_API Battle
{
public:
	Battle();
	~Battle();

	void Start();
	bool started = false;
	void Update();

	void GatherParticipants(TArray<AActor*> unitsInHex);

	//TMap<Factions, TArray<UUnitStats*>()> teams;

	FDelegateHandle beginFrameHandle;

	
};
