// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageMode.h"
#include "Troop.h"
#include "ManageTroop.generated.h"

/**
 * 
 */

UCLASS()
class CAPSTONEPROJECT_API UManageTroop : public UManageMode
{
	GENERATED_BODY()
	
public:
	ATroop* selectedTroop;
	virtual void Select(AActor* selectedObject) override;
	virtual void SwitchState() override;
	virtual void Reset() override;
	virtual void CheckSelection() override;
	virtual AActor* GetSelectedObject() override;
	
	enum SubSelections
	{
		None,
		Merge
	};
	SubSelections subSelect;

	//Commands
	void CommandToMerge(ATroop* selectedTroop, AActor* target);
	virtual void Action1() override;
	virtual void Action2() override;
	virtual void Action3() override;
	virtual void Action4() override;
	virtual void CommandAction() override;
};
