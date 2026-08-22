// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathingConditionDisplay.generated.h"

UCLASS()
class CAPSTONEPROJECT_API APathingConditionDisplay : public AActor
{
	GENERATED_BODY()
	
public:	
	APathingConditionDisplay();
	UFUNCTION(BlueprintNativeEvent) void Initialize(const TArray<FString>& conditions);
};
