// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Building.h"
#include "UI_BuildingProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUI_BuildingProgressBar : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere) UProgressBar* progressBar;
	UPROPERTY(VisibleAnywhere) ABuilding* parentBuilding;

	UFUNCTION(Blueprintable) void SetProgressBar();

	UFUNCTION(Blueprintable) bool IsBuilding();
};
