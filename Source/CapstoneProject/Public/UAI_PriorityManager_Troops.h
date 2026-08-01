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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHexSelected, ABaseHex*, hex, const TArray<FString>&, conditions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTroopSelected, ATroop*, troop, const TArray<FString>&, conditions);

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

	UPROPERTY(BlueprintAssignable) FOnHexSelected onHexSelected;
	UPROPERTY(BlueprintAssignable) FOnTroopSelected onTroopSelected;

private:
	UFaction* parentFaction;

	ABaseHex* priorityHex = nullptr;
	ATroop* priorityTroop = nullptr;

	UFUNCTION() void FindPriorityHex();
	UFUNCTION() void FindPriorityTroop();
	
	float ScoreHex(ATroop* troop, ABaseHex* hex, TArray<FString>& outScores);
	float ScoreTroop(ATroop* troop, ABaseHex* hex, TArray<FString>& outScores);
	float Score(const TArray<UAI_TroopCondition*>& conditions, ATroop* troop, ABaseHex* hex, TArray<FString>& outScores);

	UPROPERTY(EditAnywhere, Category = "Search Conditions") TArray<UAI_TroopCondition*> hexTargetConditions;
	UPROPERTY(EditAnywhere, Category = "Search Conditions") TArray<UAI_TroopCondition*> troopToTargetConditions;
};
