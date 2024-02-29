// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingAttachment.h"
#include "OutpostBarracks.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UOutpostBarracks : public UBuildingAttachment
{
	GENERATED_BODY()

public:

	UOutpostBarracks();
	TArray<ATroop*> storedTroops;
};
