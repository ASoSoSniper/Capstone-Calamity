// Fill out your copyright notice in the Description page of Project Settings.


#include "CapitolHub.h"

void ACapitolHub::BeginPlay()
{
	Super::BeginPlay();

	range = 5;
}

void ACapitolHub::UpdateResources()
{
	ABaseHex* hex = Cast<ABaseHex>(hexNav->currentHex);
	hex->UpdateFoodYield(foodYield);
	hex->UpdateProductionYield(productionYield);
	hex->UpdateEnergyYield(energyYield);
}
