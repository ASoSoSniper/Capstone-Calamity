// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageMode.h"
#include "Building.h"
#include "ManageBuilding.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UManageBuilding : public UManageMode
{
	GENERATED_BODY()
public:	
	ABuilding* selectedBuilding;

	virtual void Select(AActor* selectedObject) override;
	virtual void SwitchState() override;
	virtual void CheckSelection() override;

	virtual void Action1() override;
	virtual void Action2() override;
	virtual void Action3() override;
	virtual void Action4() override;
	virtual void Action5() override;
	//virtual void Action6() override;
	//virtual void Action7() override;
	//virtual void Action8() override;
	//virtual void Action9() override;
	//virtual void Action10() override;


	virtual void Reset() override;

	enum SubSelects
	{
		None,
		Build
	};
	SubSelects subSelect;
};
