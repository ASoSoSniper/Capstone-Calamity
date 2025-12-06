// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAI_Action.h"
#include "UAIA_Troop_Investigate.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UUAIA_Troop_Investigate : public UAI_Action
{
	GENERATED_BODY()
public:
	void ExecuteAction(IUAI_Controller* controller) override;
};
