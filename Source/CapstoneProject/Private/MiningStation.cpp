// Fill out your copyright notice in the Description page of Project Settings.


#include "MiningStation.h"
#include "UnitActions.h"

void AMiningStation::BeginPlay()
{
	Super::BeginPlay();
	currMineTime = mineTime;
}

void AMiningStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (buildState)
	{
	case Complete:
		Mine(DeltaTime);
		break;
	}
}

void AMiningStation::Mine(float& DeltaTime)
{
	if (currMineTime > 0)
	{
		currMineTime -= DeltaTime;
		return;
	}
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
	UnitActions::HarvestResources(unitStats->faction, 1, hex->hexInfo->hexStratResources);
	currMineTime = mineTime;
}
