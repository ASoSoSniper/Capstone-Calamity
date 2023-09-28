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
	UManageHex* hex = NewObject<UManageHex>();
	UManageTroop* troop = NewObject<UManageTroop>();
	UManageBuilding* building = NewObject<UManageBuilding>();

	actionStates.Add(ActionStates::None, none);
	actionStates.Add(ActionStates::HexManage, hex);
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

	//actionStates[currentActionState]->CheckSelection();
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
	if (actionStates.Num() > 0 && actionStates[currentActionState]->controller)
	{		
		if (actionStates.Find(currentActionState)) actionStates[currentActionState]->SwitchState();
		else (GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State not found")));
	}
}


void ABasePlayerController::Deselect()
{
	selectedHex = nullptr;
	selectedWorldObject = nullptr;
	actionStates[currentActionState]->Reset();
	currentActionState = ActionStates::None;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Deselected!"));
}

void ABasePlayerController::Build(UClass* prefab)
{
	if (selectedHex)
	{
		if (selectedHex->building == nullptr)
		{
			FActorSpawnParameters params;
			ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(prefab, selectedHex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
			selectedHex->building = newBuilding;

			UHexNav* hexNav = newBuilding->GetComponentByClass<UHexNav>();
			if (hexNav) hexNav->currentHex = selectedHex;

			UnitActions::AssignFaction(playerFaction, newBuilding);
		}
		else GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Hex already occupied"));
	}
}