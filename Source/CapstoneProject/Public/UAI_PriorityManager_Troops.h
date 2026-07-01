// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UAI_PriorityManager_Troops.generated.h"

class UFaction;
class UAI_HexCondition;
class UAI_TroopCondition;
class ABaseHex;
class ATroop;

USTRUCT(BlueprintType)
struct FTroopConditions
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere) TArray<UAI_TroopCondition*> conditions;
};

USTRUCT(BlueprintType)
struct FHexTargetConditions
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere) TArray<UAI_HexCondition*> conditions;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UUAI_PriorityManager_Troops : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUAI_PriorityManager_Troops();
	void Initialize(UFaction* faction);

	UFUNCTION(BlueprintCallable, BlueprintPure) ABaseHex* GetPriorityHex() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ATroop* GetPriorityTroop() const;
	UFUNCTION() void HandleOnTroopChanged();
	UFUNCTION() void HandleOnHexTargeted();

private:
	UFaction* parentFaction;

	ABaseHex* priorityHex = nullptr;
	ATroop* priorityTroop = nullptr;

	UFUNCTION() void FindPriorityHex();
	UFUNCTION() void FindPriorityTroop();
	
	float ScoreHex(ATroop* troop, ABaseHex* hex);

	UPROPERTY(EditAnywhere, Category = "Search Conditions") TArray<UAI_TroopCondition*> hexTargetConditions;
	UPROPERTY(EditAnywhere, Category = "Search Conditions") TArray<UAI_TroopCondition*> troopToTargetConditions;
};
