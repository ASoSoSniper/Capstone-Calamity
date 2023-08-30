// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

ABasePlayerController::ABasePlayerController()
{
	bEnableMouseOverEvents = true;
	bShowMouseCursor = true;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePlayerController::SetHoveredWorldObject(AActor* object)
{
	hoveredWorldObject = object;
}

void ABasePlayerController::SetSelectedWorldObject(AActor* object)
{
	selectedWorldObject = object;
	SetActionState();
}

void ABasePlayerController::SetActionState()
{
	AMovementAI* testForAI;
	switch (actionState)
	{
	case None:
		testForAI = Cast<AMovementAI>(selectedWorldObject);
		if (testForAI)
		{
			actionState = TroopManage;
			selectedTroop = testForAI;
			break;
		}
		break;
	case BaseManage:
		break;
	case TroopManage:
		selectedTroop->targetHex = selectedWorldObject;
		selectedTroop->CreatePath();
		break;
	}
}
