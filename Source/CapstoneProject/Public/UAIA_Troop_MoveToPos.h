// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UAIA_Troop_MoveToPos.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIA_Troop_MoveToPos : public UAI_Action
{
	GENERATED_BODY()
	
public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;
	virtual void SetDestination(IUAI_Controller* controller) override;
	virtual bool IsMovementAction(IUAI_Controller* controller = nullptr) const override;
};
