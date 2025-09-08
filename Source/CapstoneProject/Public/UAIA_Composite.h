// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UAIA_Composite.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIA_Composite : public UAI_Action
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;
	virtual bool IsMovementAction(IUAI_Controller* controller = nullptr) const override;
	virtual void SetDestination(IUAI_Controller* controller) override;

	virtual bool CanAbandon(IUAI_Controller* controller = nullptr) const override;
	virtual float GetAbandonTime(IUAI_Controller* controller = nullptr) const override;
	virtual bool CanUpdateDestination(IUAI_Controller* controller = nullptr) const override;

private:
	UPROPERTY(EditAnywhere, Category = "Composite Action") TArray<UAI_Action*> actions;
};
