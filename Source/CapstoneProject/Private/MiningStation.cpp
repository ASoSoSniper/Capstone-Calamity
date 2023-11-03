// Fill out your copyright notice in the Description page of Project Settings.


#include "MiningStation.h"
#include "UnitActions.h"
#include "CapstoneProjectGameModeBase.h"

void AMiningStation::BeginPlay()
{
	Super::BeginPlay();
}

void AMiningStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMiningStation::Harvest(ABaseHex* hex)
{
	switch (buildState)
	{
	case Complete:
		//UnitActions::HarvestResources(unitStats->faction, productionYield + hex->resourceBonuses[StratResources::Production], StratResources::Production);
		break;
	case Upgraded:
		break;
	}
	
}

void AMiningStation::UpdateResources()
{
	Cast<ABaseHex>(hexNav->currentHex)->UpdateProductionYield(productionYield);
}
