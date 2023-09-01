// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

ABasePlayerController::ABasePlayerController()
{
	bEnableMouseOverEvents = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
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
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("MovementAI found!"));
			actionState = TroopManage;
			selectedTroop = testForAI;
			break;
		}
		else
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("MovementAI not found."));
		break;
	case BaseManage:
		break;
	case TroopManage:
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Troop Commanded!"));
		selectedTroop->targetHex = selectedWorldObject;
		selectedTroop->CreatePath();
		break;
	}
}
