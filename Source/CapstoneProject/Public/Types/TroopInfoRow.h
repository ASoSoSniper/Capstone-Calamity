// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/UnitTypes.h"
#include "StratResources.h"
#include "TroopInfoRow.generated.h"

USTRUCT(BlueprintType)
struct FTroopInfoRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

	FTroopInfoRow() {
		resourceCosts.Add(EStratResources::Production, 25);
		attackingMultipliers.Add(UnitTypes::None, 100);
		defendingMultipliers.Add(UnitTypes::None, 100);
	}

	UPROPERTY(EditAnywhere, Category = "Identity") UnitTypes unitType;
	UPROPERTY(EditAnywhere, Category = "Identity") FString displayName;
	UPROPERTY(EditAnywhere, Category = "Identity", meta = (MultiLine = "true")) FString description;
	UPROPERTY(EditAnywhere, Category = "Identity") UTexture2D* storeIcon;
	UPROPERTY(EditAnywhere, Category = "Identity") UTexture2D* worldIcon;
	UPROPERTY(EditAnywhere, Category = "Identity") UTexture2D* worldIconHovered;

	UPROPERTY(EditAnywhere, Category = "Construction", meta = (ClampMin = 0)) TMap<EStratResources, int> resourceCosts;
	UPROPERTY(EditAnywhere, Category = "Construction", meta = (ClampMin = 0)) int ticksToBuild;

	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int health = 200;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int morale = 100;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int ticksToTravel = 10;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int vision = 1;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int baseDamage = 10;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int siegeDamage = 10;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int reinforceRate = 1;
	UPROPERTY(EditAnywhere, Category = "Stats", meta = (ClampMin = 0)) int energyUpkeepCost = 2;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = 0)) TMap<UnitTypes, int> attackingMultipliers;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = 0)) TMap<UnitTypes, int> defendingMultipliers;
};