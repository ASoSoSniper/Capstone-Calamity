// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UAI_Building_Destroy.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAI_Building_Destroy : public UAI_Action
{
	GENERATED_BODY()

public:
	virtual void ExecuteAction(IUAI_Controller* controller) override;
	
};
