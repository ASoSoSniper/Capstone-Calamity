// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "HexNav.h"

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
		if (Cast<ABaseHex>(selectedWorldObject))
		{
			selectedTroop->targetHex = selectedWorldObject;
		}
		else
		{
			UHexNav* hexNav = selectedWorldObject->GetComponentByClass<UHexNav>();
			if (hexNav)
				selectedTroop->targetHex = hexNav->currentHex;
		}
		selectedTroop->CreatePath();
		break;
	}
}

void ABasePlayerController::Build()
{
	if (actionState == None && selectedHex)
	{
		if (selectedHex->building == nullptr && buildingPrefab)
		{
			FActorSpawnParameters params;
			ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(buildingPrefab, selectedHex->buildingAnchor->GetComponentLocation(), FRotator(0,0,0), params);
			selectedHex->building = newBuilding;
			UHexNav* hexNav = newBuilding->GetComponentByClass<UHexNav>();
			if (hexNav) hexNav->currentHex = selectedHex;
		}
		else GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Hex already occupied"));
	}
	else GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Could not build"));
}

void ABasePlayerController::Deselect()
{
	selectedHex = nullptr;
	selectedTroop = nullptr;
	selectedWorldObject = nullptr;
	actionState = None;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Deselected!"));
}
