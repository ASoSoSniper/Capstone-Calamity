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
	ABaseHex* testForHex;
	switch (actionState)
	{
	case None:
		testForAI = Cast<AMovementAI>(selectedWorldObject);
		testForHex = Cast<ABaseHex>(selectedWorldObject);
		if (testForAI)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("MovementAI found!"));
			actionState = TroopManage;
			selectedTroop = testForAI;
			break;
		}
		if (testForHex)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("BaseHex found!"));
			selectedHex = testForHex;
			break;
		}
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

void ABasePlayerController::Build()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Building building!"));
}

void ABasePlayerController::Deselect()
{
	selectedHex = nullptr;
	selectedTroop = nullptr;
	selectedWorldObject = nullptr;
	actionState = None;
}
