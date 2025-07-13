// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UAI_Controller.h"
#include "UAI_Condition.h"
#include "UAI_Action.generated.h"

UCLASS(Abstract, BlueprintType)
class CAPSTONEPROJECT_API UAI_Action : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void ExecuteAction(IUAI_Controller* controller) PURE_VIRTUAL(UAI_Action::ExecuteAction, );
	TArray<UAI_Condition*>& GetConditions();
	EActionType GetDefaultActionType() const;

	virtual bool IsMovementAction() const;
	virtual void SetDestination(IUAI_Controller* controller);

	bool CanAbandon() const;
	float GetAbandonTime() const;
	bool CanUpdateDestination() const;

private:
	UPROPERTY(EditAnywhere, Category = "Conditions") TEnumAsByte<EActionType> defaultActionType;
	UPROPERTY(EditAnywhere, Category = "Conditions") TArray<UAI_Condition*> conditions;

	UPROPERTY(EditAnywhere, Category = "Execution") bool canAbandon = false;
	UPROPERTY(EditAnywhere, Category = "Execution") float abandonTime = 10.f;
	UPROPERTY(EditAnywhere, Category = "Execution") bool updateDestination = false;
};
