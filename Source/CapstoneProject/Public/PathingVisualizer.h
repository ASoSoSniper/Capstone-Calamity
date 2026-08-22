// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FactionEnum.h"
#include "PathingVisualizer.generated.h"

class APathingConditionDisplay;

UCLASS()
class CAPSTONEPROJECT_API APathingVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathingVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Tracking") TArray<EFactions> trackedFactions;
	UPROPERTY(EditAnywhere, Category = "Tracking") TSubclassOf<AActor> pathStepMarker;
	UPROPERTY(EditAnywhere, Category = "Tracking") TSubclassOf<APathingConditionDisplay> pathConditionDisplay;
	TMap<const ATroop*, TArray<AActor*>> trackedTroops;

	TArray<FString> priorityTroopConditions;
	TArray<FString> priorityHexConditions;

	TArray<FColor> arrowColors;
	int arrowColorIndex = 0;

	UFUNCTION() void BindFactions();

	UFUNCTION() void HandleOnTroopAdded(ATroop* troop);
	UFUNCTION() void HandleOnTroopRemoved(ATroop* troop);

	UFUNCTION() void HandleOnDestinationSet(const AMovementAI* troop, const ABaseHex* prevHex);

	UFUNCTION() void HandleOnTroopPrioritized(ATroop* troop, const TArray<FString>& conditions);
	UFUNCTION() void HandleOnHexPrioritized(ABaseHex* hex, const TArray<FString>& conditions);
};
