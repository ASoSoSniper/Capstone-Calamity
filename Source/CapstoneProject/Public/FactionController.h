// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UAI_Controller.h"
#include "UAI_Decider.h"
#include "FactionController.generated.h"

class UFaction;
class UUAI_PriorityManager_Hex;
class ACapstoneProjectGameModeBase;
struct FDateTickUpdate;

UCLASS()
class CAPSTONEPROJECT_API AFactionController : public AActor
{
	GENERATED_BODY()
	
public:	
	AFactionController();
	virtual void Tick(float DeltaTime) override;

	void SetFaction(UFaction* setFaction);
	UFaction* GetFactionObject() const;
	bool IsAIControlled();

	void TriggerUpdateDisplay();

	UFUNCTION(BlueprintCallable, BlueprintPure) bool UpdateDisplay();

	UFUNCTION(BlueprintCallable, BlueprintPure) UUAI_Decider* GetResourceDecider();
	UFUNCTION(BlueprintCallable, BlueprintPure) UUAI_Decider* GetTroopDecider();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UUAI_Decider* decider_Resources;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") UUAI_Decider* decider_Troops;

private:
	UFaction* faction;
	bool updateDecisionDisplay = false;

	void DateUpdate(const FDateTickUpdate& update);

#pragma region Priority Targeting
public:
	UFUNCTION() void BindHexDelegates(ABaseHex* hex, bool enable);
	UFUNCTION() void BindBuildingDelegates(ABuilding* building, bool enable);
	UFUNCTION(BlueprintCallable, BlueprintPure) ABaseHex* GetPriorityHex_Workers(EStratResources resource) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ABaseHex* GetPriorityHex_Building(SpawnableBuildings building) const;
private:
	UPROPERTY(EditAnywhere, Category = "Priority Management") UUAI_PriorityManager_Hex* priorityManager_Hex;
#pragma endregion
};
