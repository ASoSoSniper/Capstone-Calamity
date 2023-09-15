// Fill out your copyright notice in the Description page of Project Settings.


#include "Faction.h"


Faction::Faction()
{
	faction = Factions::None;
	resourceInventory.Add(StratResources::iron, 0);
	resourceInventory.Add(StratResources::copper, 0);
	resourceInventory.Add(StratResources::oil, 0);
}

Faction::~Faction()
{
}
