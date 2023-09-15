// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle.h"

//Constructor, called on creation
Battle::Battle()
{
	//Add delegate that triggers Update function every frame, required because this class does not derive from AActor
	beginFrameHandle = FCoreDelegates::OnBeginFrame.AddRaw(this, &Battle::Update);	
}

//Destructor, called on destruction
Battle::~Battle()
{
	//Remove delegate on destruction to prevent memory leakage
	FCoreDelegates::OnBeginFrame.Remove(beginFrameHandle);
}

void Battle::Start()
{
}

void Battle::Update()
{
	//Trigger Start on first frame
	if (!started)
	{
		Start();
		started = true;
	}
	
}

void Battle::GatherParticipants(TArray<AActor*> unitsInHex)
{
	for (int i = 0; i < unitsInHex.Num(); ++i)
	{
		UUnitStats* foundStats = unitsInHex[i]->FindComponentByClass<UUnitStats>();
		if (foundStats)
		{
			
		}
	}
}
