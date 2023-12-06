// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "HexNav.h"
#include "UnitActions.h"
#include "Kismet/GameplayStatics.h"

ABasePlayerController::ABasePlayerController()
{
	bEnableMouseOverEvents = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;

	UManageMode* none = NewObject<UManageMode>();
	UManageHex* hex = NewObject<UManageHex>();
	UManageTroop* troop = NewObject<UManageTroop>();
	UManageBuilding* building = NewObject<UManageBuilding>();

	none->AddToRoot();
	hex->AddToRoot();
	troop->AddToRoot();
	building->AddToRoot();

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

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}
}

void ABasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	actionStates[currentActionState]->CheckSelection();	

	if (spawner) return;

	AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
	spawner = Cast<AGlobalSpawner>(temp);
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
	if (actionStates.Num() > 0 && actionStates[currentActionState])
	{		
		if (actionStates[currentActionState]->controller)
		{
			actionStates[currentActionState]->SwitchState();
		}
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

void ABasePlayerController::Build(SpawnableBuildings building)
{
	if (selectedHex)
	{
		spawner->SpawnBuilding(playerFaction, building, selectedHex);
		return;
	}
}

TArray<int> ABasePlayerController::GetPlayerResources()
{
	TArray<int> numbers = UnitActions::GetFactionResources(playerFaction);

	return numbers;
}
