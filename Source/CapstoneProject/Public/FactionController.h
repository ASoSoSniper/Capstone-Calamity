// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UAI_Controller.h"
#include "FactionController.generated.h"

class UFaction;

UCLASS()
class CAPSTONEPROJECT_API AFactionController : public AActor, public IUAI_Controller
{
	GENERATED_BODY()
	
public:	
	AFactionController();
	virtual void Tick(float DeltaTime) override;

	void SetFaction(UFaction* setFaction);
	virtual bool IsAIControlled() override;

	void TriggerUpdateDisplay();

	UFUNCTION(BlueprintCallable, BlueprintPure) bool UpdateDisplay();
	UFUNCTION(BlueprintCallable, BlueprintPure) bool DecisionsMade() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) const FUAI_Decision& GetDecisionFromHistory(int index) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) FText GetActionFromHistory(int decision, int index) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) FText GetActionUpdateCountdown() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) FText GetActionAbandonCountdown() const;

protected:
	virtual void BeginPlay() override;
	
	virtual float GetUpdateRate() const override;

	virtual void SetBestAction(FUAI_Decision& decision) override;
	virtual const TMap<EActionType, FActionSelection>& GetActions() override;
	virtual bool DestinationReached() const override;

private:
	UFaction* faction;
	bool updateDecisionDisplay = false;

	void DateUpdateTick();

	UPROPERTY(EditAnywhere, Category = "Actions") TMap<EActionType, FActionSelection> actions;
};
