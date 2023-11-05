// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UnitActions.h"
#include "ManageMode.generated.h"

/**
 * 
 */
class ABasePlayerController;
UCLASS()
class CAPSTONEPROJECT_API UManageMode : public UDataAsset
{
	GENERATED_BODY()
	
public:
	ABasePlayerController* controller;

	virtual void Select(AActor* selectedObject);
	virtual void SwitchState();
	virtual void DestroyObject(AActor* selectedObject);	
	virtual void Reset();
	virtual void CheckSelection();
	virtual void CueActionState(ActionStates nextState);

	//Commands
	virtual void Upgrade(AActor* selectedObject);
	virtual void Action1();
	virtual void Action2();
	virtual void Action3();
	virtual void Action4();
	virtual void Action5();
	virtual void Action6();
	virtual void Action7();
	virtual void Action8();
	virtual void Action9();
	virtual void Action10();
	virtual void Action11();
	virtual void Action12();
};
