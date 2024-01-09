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

int ABasePlayerController::GetActionState()
{
	int state = 0;
	switch (currentActionState)
	{
	case ActionStates::None:
		state = 0;
		break;
	case ActionStates::HexManage:
		state = 1;
		break;
	case ActionStates::BaseManage:
		state = 2;
		break;
	case ActionStates::TroopManage:
		state = 3;
		break;
	}
	
	return state;
}

void ABasePlayerController::ForceActionState(int state)
{	
	actionStates[currentActionState]->Reset();	

	switch (ActionStates(state))
	{
	case ActionStates::None:
		break;
	case ActionStates::HexManage:
		break;
	case ActionStates::BaseManage:
		ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
		if (hex && hex->building)
		{
			actionStates[ActionStates(state)]->Select(hex->building);
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Action state forced"));
		}
		break;
	}

	currentActionState = ActionStates(state);
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

void ABasePlayerController::SetPlayerResources(TArray<int> input, bool overrideCosts)
{
	TArray<int> numbers = UnitActions::GetFactionResources(playerFaction);
	TMap<WorkerType, int> workers = UnitActions::GetFactionWorkers(playerFaction);
	for (auto worker : workers)
	{
		numbers.Add(worker.Value);
	}
	bool canAfford = true;
	for (int i = 0; i < numbers.Num(); ++i)
	{
		if (i > input.Num() - 1) continue;
		if (input[i] > 0)
		{
			numbers[i] -= input[i];
		}
		else
		{
			if (numbers[i] >= input[i])
			{
				numbers[i] += input[i];
			}
			else
			{
				if (overrideCosts)
				{
					numbers[i] = 0;
				}
				else
				{
					canAfford = false;
				}
			}
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("%d, %d, %d"), numbers[0], numbers[1], numbers[2], numbers[3]));
	if (canAfford)
		UnitActions::ConsumeSpentResources(playerFaction, numbers);
}

TArray<FBuildingDisplay> ABasePlayerController::GetBuildingDisplays()
{
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	TArray<FBuildingDisplay> buildings;
	if (!hex) return buildings;

	TArray<TerrainType> nonBuildableTerrain = UnitActions::GetNonBuildableTerrain();
	if (nonBuildableTerrain.Contains(hex->hexTerrain)) return buildings;

	for (auto building : spawner->buildingCosts)
	{
		FText name = building.Value.name;
		FText production = FText::AsNumber(building.Value.productionCost);
		FText workers = FText::AsNumber(building.Value.workerCost);
		FText buildTime = FText::AsNumber(building.Value.timeToBuild);

		buildings.Add(FBuildingDisplay{name, production, workers, buildTime});
	}

	return buildings;
}

TArray<FTroopDisplay> ABasePlayerController::GetTroopDisplays()
{
	TArray<FTroopDisplay> troops;

	for (auto troop : spawner->troopCosts)
	{
		FText name = troop.Value.name;
		UTexture2D* icon = troop.Value.icon;
		FText production = FText::AsNumber(troop.Value.productionCost);
		FText buildTime = FText::AsNumber(troop.Value.timeToBuild);
		FText population = FText::AsNumber(troop.Value.populationCost);

		troops.Add(FTroopDisplay{ name, icon, production, buildTime, population });
	}

	return troops;
}

//FOR BLUEPRINT: When player selection modes have subselection modes, this function forces the transition into the subselection
void ABasePlayerController::EnterSelectionMode(bool active)
{
	if (active && IsInBuildMode()) return;

	switch (currentActionState)
	{
	case ActionStates::HexManage:
		if (active)
		{
			//For HexManage, Action1 transitions to the building construction mode
			actionStates[currentActionState]->Action1();
		}
		else
		{
			actionStates[currentActionState]->Reset();
		}
		break;
	case ActionStates::BaseManage:
		if (active)
		{
			//For BaseManage, Action1 transitions to the troop construction mode
			actionStates[currentActionState]->Action1();
		}
		else
		{
			actionStates[currentActionState]->Reset();
		}
		break;
	}
}

bool ABasePlayerController::IsInBuildMode()
{
	return actionStates[currentActionState]->subSelect == 1;
}

//FOR BLUEPRINT: Triggered when a building is selected from the UI, identifies which building was selected and builds it accordingly
void ABasePlayerController::SelectBuilding(FText buildingName)
{
	for (auto buildings : spawner->buildingCosts)
	{
		if (buildingName.EqualTo(buildings.Value.name))
		{
			Build(buildings.Key);
			return;
		}
	}
}
//FOR BLUEPRINT: Triggered when a troop is selected from the UI, identifies which troop was selected and builds it accordingly
void ABasePlayerController::SelectTroop(FText troopName)
{
	for (auto troop : spawner->troopCosts)
	{
		if (troopName.EqualTo(troop.Value.name))
		{
			AOutpost* outpost = GetOutpost();

			if (outpost) outpost->CueTroopBuild(troop.Key);
		}
	}
}

//FOR BLUEPRINT: Converts selected hex info into a struct of FTexts to be used for UI
FHexDisplay ABasePlayerController::GetHexDisplayInfo()
{
	FHexDisplay display;
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return display;
	
	display = hex->GetDisplayInfo();

	return display;
}

//FOR BLUEPRINT: Returns the maximum number of resources the player faction can possess
int ABasePlayerController::GetResourceCap()
{	
	return UnitActions::GetResourceCap(playerFaction);
}

//FOR BLUEPRINT: Checks if selected hex has a building
bool ABasePlayerController::HexHasBuilding()
{
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);

	if (!hex) return false;

	return hex->building != nullptr;
}

//FOR BLUEPRINT: Gets info and build time for the next troop to be produced from this building
FCuedTroop ABasePlayerController::GetCuedTroop()
{
	FCuedTroop troop;

	AOutpost* outpost = GetOutpost();
	if (!outpost) return troop;

	if (outpost->cuedUnits.IsEmpty()) return troop;

	troop.troopInfo = spawner->troopCosts[outpost->cuedUnits[0]];
	troop.currentTime = outpost->currentTroopBuildTime;

	return troop;
}

//Finds whether the selected object is a hex, has a building, and whether that building is an outpost
AOutpost* ABasePlayerController::GetOutpost()
{
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return nullptr;

	ABuilding* building = hex->building;
	if (!building) return nullptr;

	AOutpost* outpost = Cast<AOutpost>(building);
	if (!outpost) return nullptr;

	return outpost;
}


