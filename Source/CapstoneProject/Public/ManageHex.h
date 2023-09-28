// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageMode.h"
#include "ManageHex.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UManageHex : public UManageMode
{
	GENERATED_BODY()
	
public:
	virtual void Select(AActor* selectedObject) override;
	virtual void SwitchState() override;
	virtual void Reset() override;

	//Commands
	virtual void Action1() override;
	virtual void Action2() override;
	virtual void Action3() override;
	virtual void Action4() override;
};
