// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalSpawner.h"
#include "CinematicPosition.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UCinematicPosition : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCinematicPosition();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FCinematicObject cinematicVars;
	UPROPERTY(EditAnywhere) FVector positionOffset;
};
