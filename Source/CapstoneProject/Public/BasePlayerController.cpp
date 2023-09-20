// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "HexNav.h"
#include "UnitActions.h"

ABasePlayerController::ABasePlayerController()
{
	bEnableMouseOverEvents = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;

	UManageMode* none = NewObject<UManageMode>();
	UManageTroop* troop = NewObject<UManageTroop>();
	UManageBuilding* building = NewObject<UManageBuilding>();

	actionStates.Add(ActionStates::None, none);
	actionStates.Add(ActionStates::TroopManage, troop);
	actionStates.Add(ActionStates::BaseManage, building);

	for (const auto& state : actionStates)
	{
		state.Value->controller = this;
	}
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
	UnitActions::SelectionIdentity objectType = UnitActions::DetermineObjectType(selectedWorldObject);

	if (actionStates.Num() > 0 && actionStates[currentActionState]->controller)
	{
		actionStates[currentActionState]->SwitchState(objectType);
	}
}

void ABasePlayerController::Build()
{
	if (currentActionState == ActionStates::None && selectedHex)
	{
		if (selectedHex->building == nullptr && buildingPrefab)
		{
			FActorSpawnParameters params;
			ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(buildingPrefab, selectedHex->buildingAnchor->GetComponentLocation(), FRotator(0,0,0), params);
			selectedHex->building = newBuilding;

			UHexNav* hexNav = newBuilding->GetComponentByClass<UHexNav>();
			if (hexNav) hexNav->currentHex = selectedHex;

			UnitActions::AssignFaction(playerFaction, newBuilding);
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
	currentActionState = ActionStates::None;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Deselected!"));
}
