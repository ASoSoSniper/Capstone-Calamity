// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "HexNav.h"
#include "UnitActions.h"
#include "TroopFactory.h"
#include "MergedArmy.h"
#include "SiegeObject.h"
#include "TroopStorage.h"
#include "CapstoneProjectGameModeBase.h"
#include "Kismet/GameplayStatics.h"

#pragma region General Logic
ABasePlayerController::ABasePlayerController()
{
	bEnableMouseOverEvents = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;

	noneMode = NewObject<UManageMode>();
	hexMode = NewObject<UManageHex>();
	troopMode = NewObject<UManageTroop>();
	buildingMode = NewObject<UManageBuilding>();
	battleMode = NewObject<UManageBattle>();

	noneMode->AddToRoot();
	hexMode->AddToRoot();
	troopMode->AddToRoot();
	buildingMode->AddToRoot();
	battleMode->AddToRoot();

	actionStates.Add(ActionStates::None, noneMode);
	actionStates.Add(ActionStates::HexManage, hexMode);
	actionStates.Add(ActionStates::TroopManage, troopMode);
	actionStates.Add(ActionStates::BaseManage, buildingMode);
	actionStates.Add(ActionStates::BattleManage, battleMode);

	for (const auto& state : actionStates)
	{
		state.Value->controller = this;
	}
}
void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	currentActionState = ActionStates::None;

	if (!playerCamera)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerMovement::StaticClass());
		playerCamera = Cast<APlayerMovement>(temp);
	}
}
void ABasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	actionStates[currentActionState]->CheckSelection();	
	//CheckForActionStates();

	if (!playerCamera)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerMovement::StaticClass());
		playerCamera = Cast<APlayerMovement>(temp);
	}
}
#pragma endregion

#pragma region Selection and Action States
void ABasePlayerController::SetHoveredWorldObject(AActor* object)
{
	hoveredWorldObject = object;
}
void ABasePlayerController::SetSelectedWorldObject(AActor* object)
{
	selectedWorldObject = object;

	SetActionState();
}

ActionStates ABasePlayerController::GetActionState()
{
	return currentActionState;
}
void ABasePlayerController::SetActionState()
{
	if (actionStates.Num() > 0 && actionStates[currentActionState])
	{
		if (actionStates[currentActionState]->controller)
		{
			actionStates[currentActionState]->SwitchState();
		}
		//else (GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State not found")));
	}
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
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Action state forced"));
		}
		break;
	}

	currentActionState = ActionStates(state);
}

AActor* ABasePlayerController::GetActionStateSelection()
{
	if (currentActionState == ActionStates::None) return nullptr;

	return actionStates[currentActionState]->GetSelectedObject();
}

void ABasePlayerController::Deselect()
{
	actionStates[currentActionState]->Reset();
	currentActionState = ActionStates::None;
	selectedHex = nullptr;
	selectedWorldObject = nullptr;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Deselected!"));
}
void ABasePlayerController::CommandAction()
{
	actionStates[currentActionState]->CommandAction();
}

//FOR BLUEPRINT: When player selection modes have subselection modes, this function forces the transition into the subselection
void ABasePlayerController::EnterSelectionMode(bool active)
{
	if (active && IsInBuildMode()) return;
	if (!active)
	{
		actionStates[currentActionState]->Reset();
		return;
	}

	switch (currentActionState)
	{
	case ActionStates::HexManage:
		if (active)
		{
			//For HexManage, Action1 transitions to the building construction mode
			actionStates[currentActionState]->Action1();
		}
		break;
	case ActionStates::BaseManage:
		if (active)
		{
			//For BaseManage, Action1 transitions to the troop construction mode
			actionStates[currentActionState]->Action1();
		}
		break;
	case ActionStates::TroopManage:
		if (active)
		{
			//For TroopManage, Action2 transitions to troop merging mode
			actionStates[currentActionState]->Action2();
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Merge mode active"));
		}
		break;
	}
}

bool ABasePlayerController::IsActionState(ActionStates state)
{
	return currentActionState == state;
}
bool ABasePlayerController::IsInBuildMode()
{
	return actionStates[currentActionState]->subSelect == 1;
}
#pragma endregion

#pragma region Cinematics and Sounds
void ABasePlayerController::PlayUISound(USoundBase* sound)
{
	if (!sound) return;

	playerCamera->audioComponent->SetSound(sound);
	playerCamera->audioComponent->Play();
}
void ABasePlayerController::PlayUITroopSound(UnitTypes unitType)
{
	if (!UITroopSounds.Contains(unitType)) return;

	playerCamera->audioComponent->SetSound(UITroopSounds[unitType]);
	playerCamera->audioComponent->Play();
}
void ABasePlayerController::PlayUITroopSelectionSound(Factions faction)
{
	if (!UITroopSelectionSounds.Contains(faction)) return;

	playerCamera->audioComponent->SetSound(UITroopSelectionSounds[faction]);
	playerCamera->audioComponent->Play();
}
void ABasePlayerController::PlayUIBuildingSound(SpawnableBuildings buildingType)
{
	if (!UIBuildingSounds.Contains(buildingType)) return;

	playerCamera->audioComponent->SetSound(UIBuildingSounds[buildingType]);
	playerCamera->audioComponent->Play();
}
void ABasePlayerController::PlayUIHexSound(TerrainType hexType, ABaseHex* hex)
{
	if (hex)
	{
		if (hex->building)
		{
			PlayUIBuildingSound(hex->building->GetBuildingType());
			return;
		}
	}

	if (!UIHexSounds.Contains(hexType)) return;

	playerCamera->audioComponent->SetSound(UIHexSounds[hexType]);
	playerCamera->audioComponent->Play();
}

void ABasePlayerController::SetCinematicObject(AActor* object, GameStates gameState)
{
	if (!ACapstoneProjectGameModeBase::cinematicObjects.Contains(object)) return;

	playerCamera->targetCinemaPos = ACapstoneProjectGameModeBase::cinematicObjects[object];
	playerCamera->controlState = APlayerMovement::Cinematic;
	playerCamera->currCinemaTime = 0;
	playerCamera->currLingerTime = 0;
	playerCamera->gameState = gameState;
	playerCamera->startPos = playerCamera->GetActorLocation();
}
#pragma endregion

#pragma region Building Commands
//FOR BLUEPRINT: Triggered when a building is selected from the UI, identifies which building was selected and builds it accordingly
void ABasePlayerController::SelectBuilding(SpawnableBuildings buildingName)
{
	if (!AGlobalSpawner::spawnerObject->buildingCosts.Contains(buildingName)) return;
	if (!selectedHex) return;

	//Fail if attempting to build a non-outpost building on foreign soil
	if (buildingName != SpawnableBuildings::Outpost && selectedHex->GetHexOwner() != playerFaction)
	{
		PlayUISound(selectFailSound);
		return;
	}

	//Trigger first build logic if building is the capitol
	if (buildingName == SpawnableBuildings::Capitol)
	{
		if (selectedHex->GetHexTerrain() == TerrainType::Ship) firstBuildPerformed = true;
		else return;
	}

	//Spawn the selected building
	AGlobalSpawner::spawnerObject->SpawnBuilding(playerFaction, buildingName, selectedHex);
}
void ABasePlayerController::DestroyBuilding()
{
	if (!selectedHex) return;

	if (selectedHex->building)
	{
		selectedHex->building->BeginDestroying();
	}
}

FBuildingOnHex ABasePlayerController::GetBuildingOnHex()
{
	FBuildingOnHex hexBuilding = FBuildingOnHex{ SpawnableBuildings::None, FText{}, nullptr };

	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return hexBuilding;

	ABuilding* building = hex->building;
	if (!building) return hexBuilding;

	hexBuilding.baseBuilding = building;
	hexBuilding.buildingType = building->GetBuildingType();
	if (AGlobalSpawner::spawnerObject->buildingCosts.Contains(hexBuilding.buildingType))
		hexBuilding.name = AGlobalSpawner::spawnerObject->buildingCosts[hexBuilding.buildingType].name;

	return hexBuilding;
}

FBuildingDisplay ABasePlayerController::GetBuildingDisplay(SpawnableBuildings building)
{
	FBuildingDisplay attachmentDisplay;
	FBuildingCost* costs;

	if (AGlobalSpawner::spawnerObject->buildingCosts.Contains(building))
		costs = &AGlobalSpawner::spawnerObject->buildingCosts[building];
	else
		return attachmentDisplay;

	attachmentDisplay.name = costs->name;
	attachmentDisplay.productionCost = FText::AsNumber(costs->productionCost);
	attachmentDisplay.workerCost = FText::AsNumber(costs->workerCost);
	attachmentDisplay.buildTime = FText::AsNumber(costs->timeToBuild);
	attachmentDisplay.buildingIcon = costs->buildingIcon;
	attachmentDisplay.buildingType = building;

	return attachmentDisplay;
}
TArray<FBuildingDisplay> ABasePlayerController::GetBuildingDisplays()
{
	TArray<FBuildingDisplay> buildings;

	for (auto& building : AGlobalSpawner::spawnerObject->buildingCosts)
	{
		if (building.Key == SpawnableBuildings::Capitol && firstBuildPerformed) continue;
		if (building.Key != SpawnableBuildings::Capitol && !firstBuildPerformed) continue;

		buildings.Add(GetBuildingDisplay(building.Key));
	}

	return buildings;
}
FBuildingTTInfo ABasePlayerController::GetBuildingTTDisplay(SpawnableBuildings buildingName)
{
	FBuildingTTInfo buildingInfo;
	FBuildingStats* buildingStats = nullptr;

	if (AGlobalSpawner::spawnerObject->buildingStats.Contains(buildingName))
		buildingStats = &AGlobalSpawner::spawnerObject->buildingStats[buildingName];
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("No tooltip data found"));
		return buildingInfo;
	}

	buildingInfo.titleTT = buildingStats->name;
	buildingInfo.descTT = buildingStats->description;

	buildingInfo.energyMod = FText::AsNumber(buildingStats->energyYield);
	buildingInfo.foodMod = FText::AsNumber(buildingStats->foodYield);
	buildingInfo.prodMod = FText::AsNumber(buildingStats->productionYield);
	buildingInfo.wealthMod = FText::AsNumber(buildingStats->wealthYield);

	buildingInfo.resourceStorageMod = FText::AsNumber(buildingStats->resourceCapIncrease);
	buildingInfo.robotStorageMod = FText::AsNumber(buildingStats->robotStorage);

	buildingInfo.diploMod = FText::AsNumber(buildingStats->diplomacy);
	buildingInfo.tradeMod = FText::AsNumber(buildingStats->trade);

	buildingInfo.siegeDamage = FText::AsNumber(buildingStats->siegeDamage);
	buildingInfo.siegeHP = FText::AsNumber(buildingStats->HP);

	buildingInfo.unrestMod = FText::AsNumber(buildingStats->unrestMod);
	buildingInfo.energyUpkeepCost = FText::AsNumber(buildingStats->energyUpkeepCost);
	buildingInfo.maxWorkers = FText::AsNumber(buildingStats->maxWorkers);

	return buildingInfo;
}

bool ABasePlayerController::ToggleFarmlandYield(bool produceFood)
{
	FBuildingOnHex buildingOnHex = GetBuildingOnHex();
	if (buildingOnHex.buildingType == SpawnableBuildings::Farmland)
	{
		if (AFarmland* farm = Cast<AFarmland>(buildingOnHex.baseBuilding))
			farm->ToggleResourcesProduced(produceFood);
	}

	return produceFood;
}
bool ABasePlayerController::FarmlandYieldsFood()
{
	FBuildingOnHex buildingOnHex = GetBuildingOnHex();
	if (buildingOnHex.buildingType == SpawnableBuildings::Farmland)
	{
		if (AFarmland* farm = Cast<AFarmland>(buildingOnHex.baseBuilding))
			farm->producingFood;
	}

	return false;
}
//FOR BLUEPRINT: Gets info and build time for the next troop to be produced from this building
FCuedTroop ABasePlayerController::GetCuedTroop()
{
	FCuedTroop troop = FCuedTroop{ FTroopCost{0,0,0}, 0 };

	FBuildingOnHex building = GetBuildingOnHex();
	if (building.buildingType != SpawnableBuildings::RobotFactory) return troop;

	if (ATroopFactory* factory = Cast<ATroopFactory>(building.baseBuilding))
	{
		troop.troopInfo = AGlobalSpawner::spawnerObject->troopCosts[factory->GetQueuedTroop()];
		troop.currentTime = factory->GetTroopBuildTime();
	}

	return troop;
}
void ABasePlayerController::ReleaseStoredTroop(int index)
{
	FBuildingOnHex building = GetBuildingOnHex();
	if (building.buildingType != SpawnableBuildings::RobotBarracks) return;

	if (ATroopStorage* storage = Cast<ATroopStorage>(building.baseBuilding))
	{
		storage->ReleaseOneTroop(index);
	}
}
void ABasePlayerController::ReleaseAllStoredTroops()
{
	FBuildingOnHex building = GetBuildingOnHex();
	if (building.buildingType != SpawnableBuildings::RobotBarracks) return;

	if (ATroopStorage* storage = Cast<ATroopStorage>(building.baseBuilding))
	{
		storage->ReleaseAllTroops();
	}
}
TArray<FTroopUIData> ABasePlayerController::GetAllStoredTroops(ATroopStorage* setStorage)
{
	TArray<FTroopUIData> data;
	ATroopStorage* storage;

	if (setStorage) storage = setStorage;
	else
	{
		FBuildingOnHex building = GetBuildingOnHex();
		if (building.buildingType != SpawnableBuildings::RobotBarracks) return data;

		storage = Cast<ATroopStorage>(building.baseBuilding);
	}

	if (storage)
	{
		TArray<FUnitData*> units = storage->GetAllStoredTroops();

		for (int i = 0; i < units.Num(); i++)
		{
			FTroopUIData UIData = GetSimpleTroopUI(units[i]);
			data.Add(UIData);
		}
	}

	return data;
}
#pragma endregion

#pragma region Hex Commands
//FOR BLUEPRINT: Converts selected hex info into a struct of FTexts to be used for UI
FHexDisplay ABasePlayerController::GetHexDisplayInfo()
{
	FHexDisplay display;
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return display;

	display = hex->GetDisplayInfo();

	return display;
}
FCurrentResourceYields ABasePlayerController::GetHexResourceYields()
{
	FCurrentResourceYields yields = FCurrentResourceYields{};

	if (selectedHex) yields = selectedHex->GetCurrentResourceYields();

	return yields;
}

FWorkersInHex ABasePlayerController::GetWorkersInHex()
{
	FWorkersInHex workers = FWorkersInHex{ 0,0,0 };

	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return workers;

	workers.humans = hex->workersInHex[WorkerType::Human];
	workers.robots = hex->workersInHex[WorkerType::Robot];
	workers.aliens = hex->workersInHex[WorkerType::Alien];
	workers.maxWorkers = hex->GetMaxWorkers();

	return workers;
}
FWorkerSliders ABasePlayerController::SetWorkerCount(FWorkerSliders sliders)
{
	if (!selectedHex) return sliders;

	selectedHex->workersInHex[WorkerType::Human] += UnitActions::SetWorkers(Factions::Human, WorkerType::Human, FMath::RoundToInt(sliders.humanWorkers * selectedHex->GetMaxWorkers()), selectedHex);
	selectedHex->workersInHex[WorkerType::Robot] += UnitActions::SetWorkers(Factions::Human, WorkerType::Robot, FMath::RoundToInt(sliders.robotWorkers * selectedHex->GetMaxWorkers()), selectedHex);
	selectedHex->workersInHex[WorkerType::Alien] += UnitActions::SetWorkers(Factions::Human, WorkerType::Alien, FMath::RoundToInt(sliders.alienWorkers * selectedHex->GetMaxWorkers()), selectedHex);

	sliders.humanDisplay = selectedHex->workersInHex[WorkerType::Human];
	sliders.robotDisplay = selectedHex->workersInHex[WorkerType::Robot];
	sliders.alienDisplay = selectedHex->workersInHex[WorkerType::Alien];

	sliders.maxWorkers = selectedHex->GetMaxWorkers();
	sliders.currWorkers = selectedHex->workersInHex[WorkerType::Human] + selectedHex->workersInHex[WorkerType::Robot] + selectedHex->workersInHex[WorkerType::Alien];

	sliders.availableHumans = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Human);
	sliders.availableRobots = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Robot);
	sliders.availableAliens = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Alien);

	return sliders;
}

//FOR BLUEPRINT: Checks if selected hex has a building
bool ABasePlayerController::HexHasBuilding()
{
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);

	if (!hex) return false;

	return hex->building != nullptr;
}
bool ABasePlayerController::UnitTypeOnHex(UnitTypes type)
{
	if (!selectedHex) return false;

	int index = 0;

	for (int i = 0; i < selectedHex->troopsInHex.Num(); i++)
	{
		if (UnitActions::ArmyContainsUnit(selectedHex->troopsInHex[i], type, index))
		{
			return true;
		}
	}

	return false;
}

bool ABasePlayerController::CanBuildOnHex()
{
	if (!selectedHex) return false;

	return selectedHex->IsBuildableTerrain();
}
bool ABasePlayerController::CanPutWorkersOnHex()
{
	if (!selectedHex) return false;

	return selectedHex->CanPutWorkersOnHex();
}
bool ABasePlayerController::HexIsDiscovered()
{
	if (!selectedHex) return false;

	return selectedHex->visibility->DiscoveredByFaction(Factions::Human);
}
bool ABasePlayerController::HexIsHumanControlled()
{
	if (!selectedHex) return false;

	return selectedHex->GetHexOwner() == Factions::Human;
}
#pragma endregion

#pragma region Player Commands
TArray<int> ABasePlayerController::GetPlayerResources()
{
	TArray<int> numbers = UnitActions::GetFactionResources(playerFaction);

	return numbers;
}
void ABasePlayerController::SetPlayerResources(int foodCost, int prodCost, int energyCost, int wealthCost, int popCost, bool overrideCosts)
{
	int canAfford = 0;

	TMap<StratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(playerFaction);
	TMap<WorkerType, int> workers = UnitActions::GetFactionWorkers(playerFaction);

	//if (foodCost >= 0 || resources[StratResources::Food] < foodCost) canAfford++;
	//if (prodCost >= 0 || resources[StratResources::Production] < prodCost) canAfford++;
	//if (energyCost >= 0 || resources[StratResources::Energy] < energyCost) canAfford++;
	//if (wealthCost >= 0 || resources[StratResources::Wealth] < wealthCost) canAfford++;
	//if (popCost >= 0 || workers[WorkerType::Human] < popCost) canAfford++;

	//if (canAfford < 5 || !overrideCosts) return;

	TMap<StratResources, int> costs;
	costs.Add(StratResources::Food, -foodCost);
	costs.Add(StratResources::Production, -prodCost);
	costs.Add(StratResources::Energy, -energyCost);
	costs.Add(StratResources::Wealth, -wealthCost);
	UnitActions::ConsumeSpentResources(playerFaction, costs);

	ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].available += popCost;
	ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].available = FMath::Max(0, ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].available);

	int remainingPopCost = workers[WorkerType::Human] + popCost;
	if (remainingPopCost >= 0) return;

	remainingPopCost = FMath::Abs(remainingPopCost);;
	int thatPopCost = remainingPopCost;

	//Kill working population
	TArray<ABaseHex*> hexesWithWorkers;
	for (ABaseHex* hex : ACapstoneProjectGameModeBase::activeFactions[playerFaction]->ownedHexes)
	{
		if (hex->workersInHex[WorkerType::Human] > 0)
			hexesWithWorkers.Add(hex);
	}

	int scanIndex = 0;

	if (hexesWithWorkers.IsEmpty()) return;

	int overloadStopper = 0;
	while (remainingPopCost != 0)
	{
		if (hexesWithWorkers[scanIndex]->workersInHex[WorkerType::Human] > 0)
		{
			hexesWithWorkers[scanIndex]->workersInHex[WorkerType::Human]--;
			remainingPopCost--;
		}

		scanIndex++;
		if (!hexesWithWorkers.IsValidIndex(scanIndex))
		{
			scanIndex = 0;
		}

		overloadStopper++;
		if (overloadStopper == 100)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not finish"));
			return;
		}
	}
	ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].working -= thatPopCost;
	ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].working =
		FMath::Max(0, ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].working);
}

//FOR BLUEPRINT: Returns the maximum number of resources the player faction can possess
int ABasePlayerController::GetResourceCap()
{
	return UnitActions::GetResourceCap(playerFaction);
}
FResourcesPerTick ABasePlayerController::GetResourcesPerTick()
{
	FResourcesPerTick resources;
	TMap<StratResources, int> playerInventory = UnitActions::GetResourcesPerTick(playerFaction);

	resources.energy = playerInventory[StratResources::Energy];
	resources.production = playerInventory[StratResources::Production];
	resources.food = playerInventory[StratResources::Food];
	resources.wealth = playerInventory[StratResources::Wealth];

	return resources;
}
FResourceGainLoss ABasePlayerController::GetResourceRates()
{
	return UnitActions::GetFaction(playerFaction)->GetResourceRates();
}

int ABasePlayerController::GetPlayerPopulation()
{
	return UnitActions::GetFactionPopulation(playerFaction);
}
int ABasePlayerController::GetStarveLevel()
{
	return UnitActions::GetFactionStarveLevel(playerFaction);
}
int ABasePlayerController::GetPowerOutageLevel()
{
	return UnitActions::GetFactionPowerOutageLevel(playerFaction);
}
FPlayerWorkers ABasePlayerController::GetPlayerWorkers()
{
	FPlayerWorkers workers;

	workers.available = ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].available;
	workers.working = ACapstoneProjectGameModeBase::activeFactions[playerFaction]->availableWorkers[WorkerType::Human].working;

	return workers;
}

void ABasePlayerController::ResourceCheats(int resourceToChange, int val)
{
	if (0 < resourceToChange && resourceToChange < 5)
	{
		UnitActions::SetFactionResources(playerFaction, StratResources(resourceToChange), val);
	}
}
#pragma endregion

#pragma region Troop Commands
//FOR BLUEPRINT: Triggered when a troop is selected from the UI, identifies which troop was selected and builds it accordingly
void ABasePlayerController::SelectTroop(UnitTypes troopName)
{
	FBuildingOnHex building = GetBuildingOnHex();

	if (building.buildingType == SpawnableBuildings::RobotFactory)
	{
		Cast<ATroopFactory>(building.baseBuilding)->QueueTroopBuild(troopName);
	}
}
void ABasePlayerController::SplitArmyInHalf()
{
	AActor* actor = GetActionStateSelection();
	if (!actor) return;

	ATroop* troop = Cast<ATroop>(actor);
	if (!troop) return;

	troop->SetBestAction(troop_HalveArmy);
}
void ABasePlayerController::ExtractUnitFromArmy(UnitTypes unitType)
{
	AActor* actor = GetActionStateSelection();
	if (!actor) return;

	ATroop* troop = Cast<ATroop>(actor);
	if (!troop) return;

	if (troop->GetUnitData()->GetUnitType() != UnitTypes::Army) return;
	AMergedArmy* army = Cast<AMergedArmy>(troop);

	if (army) army->ExtractOneUnit(unitType);
}

bool ABasePlayerController::CheckIfArmy()
{
	AActor* Actor = GetActionStateSelection();

	if (ATroop* Army = Cast<ATroop>(Actor))
	{
		return Army->GetUnitData()->GetUnitType() == UnitTypes::Army;
	}

	return false;
}
TArray<FTroopDisplay> ABasePlayerController::GetTroopDisplays()
{
	TArray<FTroopDisplay> troops;

	for (auto& troop : AGlobalSpawner::spawnerObject->troopCosts)
	{
		if (troop.Key == UnitTypes::None || troop.Key == UnitTypes::Army) continue;

		FText name = troop.Value.name;
		UTexture2D* icon = troop.Value.icon;
		FText production = FText::AsNumber(troop.Value.productionCost);
		FText buildTime = FText::AsNumber(troop.Value.timeToBuild);
		FText population = FText::AsNumber(troop.Value.populationCost);
		UnitTypes unitType = troop.Key;

		troops.Add(FTroopDisplay{ name, icon, production, buildTime, population, unitType });
	}

	return troops;
}
FArmyDisplay ABasePlayerController::DisplaySelectedUnit()
{
	FArmyDisplay display;
	AActor* actor = GetActionStateSelection();

	if (!actor) return display;
	ATroop* troop = Cast<ATroop>(actor);

	if (!troop) return display;

	display = DisplayUnit(troop->GetUnitData());
	return display;
}
FArmyDisplay ABasePlayerController::DisplayUnit(FUnitData* unit) const
{
	FArmyDisplay display;
	FTroopStats* stats = new FTroopStats();

	UnitTypes unitType = unit->GetUnitType();

	if (AGlobalSpawner::spawnerObject->troopStats.Contains(unitType))
	{
		stats = &AGlobalSpawner::spawnerObject->troopStats[unitType];
	}

	if (unitType != UnitTypes::Army)
	{
		display.name = stats->title;
		display.icon = stats->icon;
		display.iconHovered = stats->iconHovered;
	}
	else
	{
		UnitTypes chosenType = unit->GetLargestUnitQuantity();

		display.name = unit->GetArmyName();
		display.icon = AGlobalSpawner::spawnerObject->troopStats[chosenType].icon;
		display.iconHovered = AGlobalSpawner::spawnerObject->troopStats[chosenType].iconHovered;
	}

	display.HP = unit->GetCurrentHP();
	display.HPMax = unit->GetMaxHP();
	display.morale = unit->GetCurrentMorale();
	display.moraleMax = unit->GetMaxMorale();

	display.damage = unit->GetDamage();
	display.buildingDamage = unit->GetSiegePower();

	display.speed = unit->GetSpeed();
	display.energyCost = unit->GetEnergyUpkeep();

	return display;
}
FArmyMenuInfo ABasePlayerController::DisplayArmyMenu()
{
	FArmyMenuInfo display{ FText::FromString("Army Name"), nullptr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	AActor* actor = GetActionStateSelection();

	if (!actor) return display;
	ATroop* army = Cast<ATroop>(actor);
	if (!army) return display;

	FUnitData* data = army->GetUnitData();
	UnitTypes unitType = data->GetUnitType();

	if (unitType != UnitTypes::Army)
	{
		display.name = AGlobalSpawner::spawnerObject->troopStats[unitType].title;
		display.icon = AGlobalSpawner::spawnerObject->troopCosts[unitType].icon;
	}
	else
	{
		UnitTypes chosenType = data->GetLargestUnitQuantity();

		display.name = AGlobalSpawner::spawnerObject->troopStats[chosenType].title;
		display.icon = AGlobalSpawner::spawnerObject->troopCosts[chosenType].icon;
	}

	display.HP = data->GetCurrentHP();
	display.HPMax = data->GetMaxHP();
	display.morale = data->GetCurrentMorale();
	display.moraleMax = data->GetMaxMorale();

	display.damage = data->GetDamage();
	display.buildingDamage = data->GetSiegePower();

	display.speed = data->GetSpeed();
	display.energyUpkeepCost = data->GetEnergyUpkeep();

	display.vision = data->GetVision();

	TMap<UnitTypes, FUnitComposition> units = data->GetUnitComposition();

	if (unitType == UnitTypes::Army)
	{
		int totalUnits = 0;
		for (auto& unit : units)
		{
			totalUnits += unit.Value.quantity;

			display.attackvsInfantry += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsInfantry;
			display.attackvsCavalry += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsCavalry;
			display.attackvsScout += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsScout;
			display.attackvsRanged += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsRanged;
			display.attackvsShielder += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsShielder;
			display.attackvsSettler += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].attackvsSettler;

			display.defendvsInfantry += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsInfantry;
			display.defendvsCavalry += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsCavalry;
			display.defendvsScout += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsScout;
			display.defendvsRanged += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsRanged;
			display.defendvsShielder += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsShielder;
			display.defendvsSettler += unit.Value.quantity * AGlobalSpawner::spawnerObject->troopStats[unit.Key].defendvsSettler;
		}

		display.attackvsInfantry /= totalUnits;
		display.attackvsCavalry /= totalUnits;
		display.attackvsScout /= totalUnits;
		display.attackvsRanged /= totalUnits;
		display.attackvsShielder /= totalUnits;
		display.attackvsSettler /= totalUnits;

		display.defendvsInfantry /= totalUnits;
		display.defendvsCavalry /= totalUnits;
		display.defendvsScout /= totalUnits;
		display.defendvsRanged /= totalUnits;
		display.defendvsShielder /= totalUnits;
		display.defendvsSettler /= totalUnits;

		display.infantryNum = units[UnitTypes::Infantry].quantity;
		display.cavalryNum = units[UnitTypes::Cavalry].quantity;
		display.scoutNum = units[UnitTypes::Scout].quantity;
		display.rangedNum = units[UnitTypes::Ranged].quantity;
		display.shielderNum = units[UnitTypes::Shielder].quantity;
		display.settlerNum = units[UnitTypes::Settler].quantity;
	}
	else
	{
		display.attackvsInfantry = AGlobalSpawner::spawnerObject->troopStats[unitType].attackvsInfantry;
		display.attackvsCavalry = AGlobalSpawner::spawnerObject->troopStats[unitType].attackvsCavalry;
		display.attackvsScout = AGlobalSpawner::spawnerObject->troopStats[unitType].attackvsScout;
		display.attackvsRanged = AGlobalSpawner::spawnerObject->troopStats[unitType].attackvsRanged;
		display.attackvsShielder = AGlobalSpawner::spawnerObject->troopStats[unitType].attackvsShielder;
		display.attackvsSettler = AGlobalSpawner::spawnerObject->troopStats[unitType].attackvsSettler;

		display.defendvsInfantry = AGlobalSpawner::spawnerObject->troopStats[unitType].defendvsInfantry;
		display.defendvsCavalry = AGlobalSpawner::spawnerObject->troopStats[unitType].defendvsCavalry;
		display.defendvsScout = AGlobalSpawner::spawnerObject->troopStats[unitType].defendvsScout;
		display.defendvsRanged = AGlobalSpawner::spawnerObject->troopStats[unitType].defendvsRanged;
		display.defendvsShielder = AGlobalSpawner::spawnerObject->troopStats[unitType].defendvsShielder;
		display.defendvsSettler = AGlobalSpawner::spawnerObject->troopStats[unitType].defendvsSettler;

		switch (unitType)
		{
		case UnitTypes::Infantry:
			display.infantryNum = 1;
			break;
		case UnitTypes::Cavalry:
			display.cavalryNum = 1;
			break;
		case UnitTypes::Scout:
			display.scoutNum = 1;
			break;
		case UnitTypes::Ranged:
			display.rangedNum = 1;
			break;
		case UnitTypes::Shielder:
			display.shielderNum = 1;
			break;
		case UnitTypes::Settler:
			display.settlerNum = 1;
			break;
		}
	}


	return display;
}

FTroopUIData ABasePlayerController::GetSimpleTroopUI(FUnitData* data)
{
	FTroopUIData display;

	display.troopName = data->GetArmyName();
	display.healthPercent = data->GetHPAlpha();
	display.moralePercent = data->GetMoraleAlpha();
	display.icon = GetTroopIcon(data);

	return display;
}

FTroopTTInfo ABasePlayerController::GetTroopTTDisplay(FText troopName)
{
	FTroopTTInfo info = FTroopTTInfo{};
	FTroopStats troopStats;

	for (auto& troop : AGlobalSpawner::spawnerObject->troopStats)
	{
		if (troopName.EqualTo(troop.Value.title))
		{
			troopStats = troop.Value;
			break;
		}
	}

	info.titleTT = troopStats.title;
	info.descTT = troopStats.desc;
	info.HP = FText::AsNumber(troopStats.HP);
	info.Speed = FText::AsNumber(troopStats.speed);
	info.Damage = FText::AsNumber(troopStats.damage);
	info.siegePower = FText::AsNumber(troopStats.siegePower);
	info.vision = FText::AsNumber(troopStats.vision);
	info.morale = FText::AsNumber(troopStats.morale);
	info.reinforceRate = FText::AsNumber(troopStats.reinforceRate);
	info.energyUpkeepCost = FText::AsNumber(troopStats.energyUpkeepCost);

	info.attackvsInfantry = FText::AsNumber(troopStats.attackvsInfantry);
	info.attackvsCavalry = FText::AsNumber(troopStats.attackvsCavalry);
	info.attackvsRanged = FText::AsNumber(troopStats.attackvsRanged);
	info.attackvsShielder = FText::AsNumber(troopStats.attackvsShielder);
	info.attackvsScout = FText::AsNumber(troopStats.attackvsScout);
	info.attackvsSettler = FText::AsNumber(troopStats.attackvsSettler);

	info.defendvsInfantry = FText::AsNumber(troopStats.defendvsInfantry);
	info.defendvsCavalry = FText::AsNumber(troopStats.defendvsCavalry);
	info.defendvsRanged = FText::AsNumber(troopStats.defendvsRanged);
	info.defendvsShielder = FText::AsNumber(troopStats.defendvsShielder);
	info.defendvsScout = FText::AsNumber(troopStats.defendvsScout);
	info.defendvsSettler = FText::AsNumber(troopStats.defendvsSettler);

	return info;
}

void ABasePlayerController::ChangeArmyName(FString newName)
{
	AActor* actor = GetActionStateSelection();
	if (!actor) return;

	ATroop* troop = Cast<ATroop>(actor);
	if (!troop) return;

	troop->GetUnitData()->GenerateArmyName(newName);
}
FString ABasePlayerController::GetTroopNameRaw()
{
	AActor* actor = GetActionStateSelection();

	if (ATroop* troop = Cast<ATroop>(actor))
	{
		return troop->GetUnitData()->GetNameRaw();
	}

	return "";
}
float ABasePlayerController::GetTroopHPAlpha(FUnitData* unit)
{
	if (unit) return unit->GetHPAlpha();

	AActor* actor = GetActionStateSelection();

	if (ATroop* troop = Cast<ATroop>(actor))
	{
		return troop->GetUnitData()->GetHPAlpha();
	}

	return 0.0f;
}
float ABasePlayerController::GetTroopMoraleAlpha(FUnitData* unit)
{
	if (unit) return unit->GetMoraleAlpha();

	AActor* actor = GetActionStateSelection();

	if (ATroop* troop = Cast<ATroop>(actor))
	{
		return troop->GetUnitData()->GetMoraleAlpha();
	}

	return 0.0f;
}
UTexture2D* ABasePlayerController::GetTroopIcon(FUnitData* unit)
{
	UnitTypes chosenUnit = unit->GetLargestUnitQuantity();

	return AGlobalSpawner::spawnerObject->troopStats[chosenUnit].icon;
}
#pragma endregion

#pragma region Battle Commands
FTroopArmyDisplay ABasePlayerController::GetBattleUnit(int group)
{
	FTroopArmyDisplay unit = FTroopArmyDisplay{ FText::FromString(TEXT("Goofy ah")), 0, 0, 0, 0, 0, FText::FromString(TEXT("0")), nullptr, 0,0,0,0,0,0 };

	ABattleObject* battleObject = Cast<ABattleObject>(GetActionStateSelection());
	if (!battleObject) return unit;

	unit.hexIcon = battleObject->GetHex()->GetDisplayInfo().icon;
	unit.dieRoll = group == 0 ? battleObject->GetGroup1Die() : battleObject->GetGroup2Die();
	unit.name = FText::FromString(TEXT("Goofy ah"));
	unit.defenderBonus = FText::AsNumber(battleObject->GetHex()->GetDefenderBonus());

	if (battleObject->currentBattle.Group1.IsEmpty() && group == 0) return unit;
	if (battleObject->currentBattle.Group2.IsEmpty() && group == 1) return unit;

	battleObject->GetGroupHealthAndMorale(group, unit.HP, unit.maxHP, unit.morale, unit.maxMorale);

	if (battleObject->groupCompositions[group].Contains(UnitTypes::Infantry))
		unit.infantryQuantity = battleObject->groupCompositions[group][UnitTypes::Infantry].quantity;
	else
		unit.infantryQuantity = 0;

	if (battleObject->groupCompositions[group].Contains(UnitTypes::Cavalry))
		unit.cavalryQuantity = battleObject->groupCompositions[group][UnitTypes::Cavalry].quantity;
	else
		unit.cavalryQuantity = 0;

	if (battleObject->groupCompositions[group].Contains(UnitTypes::Scout))
		unit.scoutQuantity = battleObject->groupCompositions[group][UnitTypes::Scout].quantity;
	else
		unit.scoutQuantity = 0;

	if (battleObject->groupCompositions[group].Contains(UnitTypes::Ranged))
		unit.rangedQuantity = battleObject->groupCompositions[group][UnitTypes::Ranged].quantity;
	else
		unit.rangedQuantity = 0;

	if (battleObject->groupCompositions[group].Contains(UnitTypes::Shielder))
		unit.shielderQuantity = battleObject->groupCompositions[group][UnitTypes::Shielder].quantity;
	else
		unit.shielderQuantity = 0;

	if (battleObject->groupCompositions[group].Contains(UnitTypes::Settler))
		unit.settlerQuantity = battleObject->groupCompositions[group][UnitTypes::Settler].quantity;
	else
		unit.settlerQuantity = 0;

	return unit;
}
TArray<FTroopArmyDisplay> ABasePlayerController::GetBattleUnits(int group)
{
	TArray<FTroopArmyDisplay> battleUnits = TArray<FTroopArmyDisplay>();

	ABattleObject* battleObject = Cast<ABattleObject>(selectedWorldObject);
	if (!battleObject) return battleUnits;

	for (auto& unitType : battleObject->groupCompositions[group])
	{
		FTroopArmyDisplay unit;
		/*unit.unitType = unitType.Key;
		unit.name = spawner->troopCosts[unitType.Key].name;
		unit.icon = spawner->troopCosts[unitType.Key].icon;
		unit.quantity = FText::AsNumber(unitType.Value.quantity);
		unit.dieRoll = group == 0 ? battleObject->group1Die : battleObject->group2Die;
		unit.defenderBonus = FText::AsNumber(battleObject->hex->defenderBonus);*/

		battleUnits.Add(unit);
	}

	return battleUnits;
}

void ABasePlayerController::RetreatFromBattle()
{
	AActor* Actor = GetActionStateSelection();

	if (ABattleObject* Battle = Cast<ABattleObject>(Actor))
	{
		Battle->FleeFromBattle(Factions::Human);
	}
}

bool ABasePlayerController::BattleIsSiege()
{
	if (currentActionState != ActionStates::BattleManage) return false;

	AActor* actor = actionStates[currentActionState]->GetSelectedObject();
	if (ASiegeObject* siege = Cast<ASiegeObject>(actor))
	{
		return true;
	}

	return false;
}
FSiegeBuildingInfo ABasePlayerController::GetSiegeBuildingInfo()
{
	FSiegeBuildingInfo info = FSiegeBuildingInfo{ 0,0,1,0 };

	if (currentActionState != ActionStates::BattleManage) return info;

	AActor* actor = actionStates[currentActionState]->GetSelectedObject();
	if (!actor) return info;

	ASiegeObject* siege = Cast<ASiegeObject>(actor);
	if (!siege) return info;

	FBuildingStats stats = AGlobalSpawner::spawnerObject->buildingStats[siege->GetBuilding()->GetBuildingType()];
	FUnitData* buildingData = siege->GetBuilding()->GetUnitData();

	info.buildingName = stats.name;
	info.buildingType = siege->GetBuilding()->GetBuildingType();
	info.buildingIcon = stats.buildingIcon;

	info.currentHealth = buildingData->GetCurrentHP();
	info.maxHealth = buildingData->GetMaxHP();

	info.buildingHealthPercent = (float)info.currentHealth / (float)info.maxHealth;

	info.buildingDamage = buildingData->GetDamage();

	return info;
}

bool ABasePlayerController::BattleGroupIsHuman(int group)
{
	ABattleObject* battleObject = Cast<ABattleObject>(selectedWorldObject);

	if (group == 0)
	{
		if (battleObject->currentBattle.Group1.Contains(Factions::Human))
		{
			return true;
		}
	}
	else
	{
		if (battleObject->currentBattle.Group2.Contains(Factions::Human))
		{
			return true;
		}
	}
	return false;
}
#pragma endregion