// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UAI_Faction_Wait.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAI_Faction_Wait : public UAI_Action
{
	GENERATED_BODY()
public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;
	virtual bool IsMovementAction(IUAI_Controller* controller = nullptr) const override;
};
