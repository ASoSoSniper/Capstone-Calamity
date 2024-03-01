// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outpost.h"
#include "AlienCity.generated.h"

class UCityEmbassy;
class UCityPoliceStation;
class UCityTradeOutpost;
class UOutpostDefenses;
class UOutpostStorage;
class UOutpostBarracks;
class UOutpostTroopFactory;
class UBuildingAttachment;

UCLASS()
class CAPSTONEPROJECT_API AAlienCity : public AOutpost
{
	GENERATED_BODY()
	
public:
	AAlienCity();

	UCityEmbassy* cityEmbassy;
	UCityPoliceStation* cityPoliceStation;
	UCityTradeOutpost* cityTradeOutpost;

	virtual UBuildingAttachment* GetAttachment(BuildingAttachments attachment) override;
};
