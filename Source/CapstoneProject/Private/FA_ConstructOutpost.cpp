// Fill out your copyright notice in the Description page of Project Settings.


#include "FA_ConstructOutpost.h"

/*
	--Construct Outpost--

	Score Factors:
		- Settler exists
		- Settler is positioned a minimum distance from the nearest owned outpost
		- Settler is not in the LOS of an enemy outpost

	Action:
		- Create an outpost from an existing settler
*/

FA_ConstructOutpost::FA_ConstructOutpost()
{
}

FA_ConstructOutpost::~FA_ConstructOutpost()
{
}

int FA_ConstructOutpost::ScoreAction(Faction_AIController& controller)
{
	return 0;
}

void FA_ConstructOutpost::PerformAction(Faction_AIController& controller)
{
}
