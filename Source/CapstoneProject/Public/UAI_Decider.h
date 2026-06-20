// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UAI_Controller.h"
#include "UAI_Decider.generated.h"

class UFaction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDecisionMade);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UUAI_Decider : public UActorComponent, public IUAI_Controller
{
	GENERATED_BODY()

public:	
	UUAI_Decider();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool IsAIControlled() override;

	void Initialize(UFaction* inFaction);

	UPROPERTY(BlueprintAssignable, Category = "Events") FOnDecisionMade onDecisionMade;

	UFUNCTION(BlueprintCallable, BlueprintPure) const FUAI_Decision& GetDecisionFromHistory(int index) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) FText GetActionFromHistory(int decision, int index) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) FText GetActionUpdateCountdown() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) FText GetActionAbandonCountdown() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) bool DecisionsMade() const;

protected:
	virtual void BeginPlay() override;

	virtual float GetUpdateRate() const override;

	virtual void SetBestAction(FUAI_Decision& decision) override;
	virtual const TMap<EActionType, FActionSelection>& GetActions() override;
	virtual bool DestinationReached() const override;

private:	
	UPROPERTY(EditAnywhere, Category = "Actions") TMap<EActionType, FActionSelection> actions;
	UPROPERTY(EditAnywhere, Category = "Actions") float updateTime = 1.f;
};
