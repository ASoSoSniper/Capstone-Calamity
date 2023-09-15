// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "HexNav.h"
#include "UnitActions.h"

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
	SelectionIdentity objectType = DetermineObjectType(selectedWorldObject);
	bool hostileTarget;

	switch (actionState)
	{
	// ***When the prior selected object was null or a hex***
	case None:

		//Current selected object type:
		switch (objectType.type)
		{
			//If hex, save in hex pointer
		case Hex:
			selectedHex = objectType.hex;
			break;
			//If troop, save in troop pointer and switch to TroopManage state
		case MoveAI:
			selectedTroop = objectType.moveAI;
			actionState = TroopManage;
			break;
			//If building, switch to BaseManage state
		case Building:
			actionState = BaseManage;
			break;
		}
		break;
	// ***When the prior selected object was a commandable/movable unit***
	case TroopManage:
		hostileTarget = UnitActions::IsHostileTarget(selectedTroop, selectedWorldObject);
		//Current selected object type:
		switch (objectType.type)
		{
			//If hex, set troop's destination to the hex
		case Hex:
			selectedTroop->hexNav->targetHex = selectedWorldObject;
			break;

			//If troop (and hostile), set troop's destination to that troop's current hex
		case MoveAI:
			if (hostileTarget)
			{
				UHexNav* hexNav = selectedWorldObject->GetComponentByClass<UHexNav>();
				if (hexNav)
					selectedTroop->hexNav->targetHex = hexNav->currentHex;
			}
			else
			{
				//else set selected troop to that (friendly) troop
				selectedTroop = objectType.moveAI;
				return;
			}
			break;
			//If building (and hostile), set troop's destination to that building's current hex
		case Building:
			if (hostileTarget)
			{
				UHexNav* hexNav = selectedWorldObject->GetComponentByClass<UHexNav>();
				if (hexNav)
					selectedTroop->hexNav->targetHex = hexNav->currentHex;
			}
			else
			{
				//else switch to BaseManage state
				actionState = BaseManage;
				return;
			}
			break;
		}
		
		selectedTroop->CreatePath();
		break;
	// ***When the prior selected object was a commandable building***
	case BaseManage:

		//Current selected object type:
		switch (objectType.type)
		{
			//If hex, switch to None state
		case Hex:
			selectedHex = objectType.hex;
			actionState = None;
			break;
			//If troop, set to TroopManage state
		case MoveAI:
			selectedTroop = objectType.moveAI;
			actionState = TroopManage;
			break;

		case Building:
			break;
		}
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
	actionState = None;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Deselected!"));
}

ABasePlayerController::SelectionIdentity ABasePlayerController::DetermineObjectType(AActor* object)
{
	SelectionIdentity Results{nullptr, nullptr, nullptr, NoType};
	
	ABaseHex* testForHex = Cast<ABaseHex>(selectedWorldObject);
	if (testForHex)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("BaseHex found!"));

		Results.type = Hex;
		Results.hex = testForHex;
		return Results;
	}

	AMovementAI* testForAI = Cast<AMovementAI>(selectedWorldObject);
	if (testForAI)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("MovementAI found!"));
		
		Results.type = MoveAI;
		Results.moveAI = testForAI;
		return Results;
	}

	ABuilding* testForBuilding = Cast<ABuilding>(selectedWorldObject);
	if (testForBuilding)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Building found!"));

		Results.building = testForBuilding;
		Results.type = Building;
		return Results;
	}
	return Results;
}
