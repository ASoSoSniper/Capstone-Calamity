// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "BaseHex.h"
#include "Outpost.generated.h"

class UOutpostStorage;
class UBuildingAttachment;

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API AOutpost : public ABuilding
{
	GENERATED_BODY()

public:

	AOutpost();

	UPROPERTY(EditAnywhere) int range = 4;
	TArray<ABaseHex*> ClaimLand();

	TArray<ABaseHex*> ScanHex(ABaseHex* hex);
	
	UPROPERTY(EditAnywhere)
		float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere)
		float traceLength = 50.f;
	UPROPERTY(VisibleAnywhere) TArray<ABaseHex*> claimedHexes;
	
	void BuildingAction() override;

	enum BuildingAttachments
	{
		None,
		Storage,
		DefenseStation,
		RobotFactory,
		Barracks
	};
	void BuildAttachment(BuildingAttachments attachment);

	UPROPERTY(EditAnywhere) UOutpostStorage* storageBuilding;
};
