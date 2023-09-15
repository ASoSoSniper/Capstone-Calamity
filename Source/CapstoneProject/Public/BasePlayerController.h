// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MovementAI.h"
#include "BaseHex.h"
#include "Building.h"
#include "Faction.h"
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
	UPROPERTY(VisibleAnywhere) AActor* hoveredWorldObject;

	//Interactable object the player has clicked to select
	UPROPERTY(VisibleAnywhere) AActor* selectedWorldObject;

	enum ObjectTypes
	{
		NoType,
		Hex,
		MoveAI,
		Building
	};
	struct SelectionIdentity
	{
		ABaseHex* hex;
		AMovementAI* moveAI;
		ABuilding* building;
		ObjectTypes type;
	};

	//Additional info on selectedWorldObject if it is identified as a movable unit
	UPROPERTY(VisibleAnywhere) ABaseHex* selectedHex;
	UPROPERTY(VisibleAnywhere) AMovementAI* selectedTroop;
public:	

	enum ActionStates
	{
		None,
		BaseManage,
		TroopManage
	};
	ActionStates actionState;

	Factions playerFaction = Factions::Human;

	void SetHoveredWorldObject(AActor* object);
	void SetSelectedWorldObject(AActor* object);
	void SetActionState();
	void Build();
	void Deselect();
	
	SelectionIdentity DetermineObjectType(AActor* object);

	UPROPERTY(EditAnywhere) TSubclassOf<class ABuilding> buildingPrefab;
};
