// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outpost.h"
#include "AlienCity.generated.h"

UCLASS()
class CAPSTONEPROJECT_API AAlienCity : public AOutpost
{
	GENERATED_BODY()
	
public:
	AAlienCity();

	virtual UStaticMesh* LoadFinishedModel() override;
};
