// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MovementAI.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */

UCLASS()
class CAPSTONEPROJECT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();

protected:

	virtual void BeginPlay() override;
private:

	virtual void Tick(float DeltaTime) override;
	
	//Interactable object the cursor hovers over
	AActor* hoveredWorldObject;

	//Interactable object the player has clicked to select
	AActor* selectedWorldObject;
	
	//Additional info on selectedWorldObject if it is identified as a movable unit
	AMovementAI* selectedTroop;
public:	

	enum ActionStates
	{
		None,
		BaseManage,
		TroopManage
	};
	ActionStates actionState;

	void SetHoveredWorldObject(AActor* object);
	void SetSelectedWorldObject(AActor* object);
	void SetActionState();
};
