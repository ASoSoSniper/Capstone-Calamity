// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerPlant.h"

void APowerPlant::UpdateResources()
{
	Cast<ABaseHex>(hexNav->currentHex)->UpdateEnergyYield(energyYield);
}
