// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManageMode.h"
#include "BattleObject.h"
#include "ManageBattle.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API UManageBattle : public UManageMode
{
	GENERATED_BODY()
	
public:
	ABattleObject* selectedBattle;

	virtual void Select(AActor* selectedObject) override;
	virtual void SwitchState() override;
	virtual void Reset() override;
	virtual void CheckSelection() override;
	virtual AActor* GetSelectedObject() override;

	virtual void Action1() override;
};
