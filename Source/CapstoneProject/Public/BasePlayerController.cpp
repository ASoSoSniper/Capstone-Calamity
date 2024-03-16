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

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}

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

	if (!spawner)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), AGlobalSpawner::StaticClass());
		spawner = Cast<AGlobalSpawner>(temp);
	}

	if (!playerCamera)
	{
		AActor* temp = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerMovement::StaticClass());
		playerCamera = Cast<APlayerMovement>(temp);
	}
}
void ABasePlayerController::PlayUISound(USoundBase* sound)
{
	playerCamera->audioComponent->SetSound(sound);
	playerCamera->audioComponent->Play();
}

TArray<FTroopArmyDisplay> ABasePlayerController::GetBattleUnits(int group)
{
	TArray<FTroopArmyDisplay> battleUnits = TArray<FTroopArmyDisplay>();

	ABattleObject* battleObject = Cast<ABattleObject>(selectedWorldObject);
	if (!battleObject) return battleUnits;

	for (auto& unitType : battleObject->groupCompositions[group])
	{
		FTroopArmyDisplay unit;
		unit.unitType = unitType.Key;
		unit.name = spawner->troopCosts[unitType.Key].name;
		unit.icon = spawner->troopCosts[unitType.Key].icon;
		unit.quantity = FText::AsNumber(unitType.Value.quantity);
		unit.dieRoll = group == 0 ? FText::AsNumber(battleObject->group1Die) : FText::AsNumber(battleObject->group2Die);
		unit.defenderBonus = FText::AsNumber(battleObject->hex->defenderBonus);

		battleUnits.Add(unit);
	}
	
	return battleUnits;
}

FTroopArmyDisplay ABasePlayerController::GetBattleUnit(int group, UnitTypes type)
{
	FTroopArmyDisplay unit;

	ABattleObject* battleObject = Cast<ABattleObject>(selectedWorldObject);
	if (!battleObject) return unit;

	ABattleObject::UnitComposition composition = battleObject->groupCompositions[group][type];

	unit.unitType = type;
	unit.name = spawner->troopCosts[type].name;
	unit.icon = spawner->troopCosts[type].icon;
	unit.quantity = FText::AsNumber(composition.quantity);
	unit.dieRoll = group == 0 ? FText::AsNumber(battleObject->group1Die) : FText::AsNumber(battleObject->group2Die);
	unit.defenderBonus = FText::AsNumber(battleObject->hex->defenderBonus);

	return unit;
}

void ABasePlayerController::PlayUITroopSound(UnitTypes unitType)
{
	if (!UITroopSounds.Contains(unitType)) return;

	playerCamera->audioComponent->SetSound(UITroopSounds[unitType]);
	playerCamera->audioComponent->Play();
}

void ABasePlayerController::PlayUIBuildingSound(SpawnableBuildings buildingType)
{
	if (!UIBuildingSounds.Contains(buildingType)) return;

	playerCamera->audioComponent->SetSound(UIBuildingSounds[buildingType]);
	playerCamera->audioComponent->Play();
}

void ABasePlayerController::PlayUIAttachmentSound(BuildingAttachments attachmentType)
{
	if (!UIAttachmentSounds.Contains(attachmentType)) return;

	playerCamera->audioComponent->SetSound(UIAttachmentSounds[attachmentType]);
	playerCamera->audioComponent->Play();
}

void ABasePlayerController::PlayUIHexSound(TerrainType hexType)
{
	if (!UIHexSounds.Contains(hexType)) return;

	playerCamera->audioComponent->SetSound(UIHexSounds[hexType]);
	playerCamera->audioComponent->Play();
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
		//else (GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("State not found")));
	}
}


void ABasePlayerController::Deselect()
{
	actionStates[currentActionState]->Reset();
	currentActionState = ActionStates::None;
	selectedHex = nullptr;
	selectedWorldObject = nullptr;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, TEXT("Deselected!"));
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
	case ActionStates::BattleManage:
		state = 4;
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
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Action state forced"));
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
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, FString::Printf(TEXT("%d, %d, %d"), numbers[0], numbers[1], numbers[2], numbers[3]));
	if (canAfford)
		UnitActions::ConsumeSpentResources(playerFaction, numbers);
}

int ABasePlayerController::GetPlayerPopulation()
{
	return UnitActions::GetFactionPopulation(playerFaction);
}

FWorkersInHex ABasePlayerController::GetWorkersInHex()
{
	FWorkersInHex workers = FWorkersInHex{ 0,0,0 };

	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return workers;

	workers.humans = hex->workersInHex[WorkerType::Human];
	workers.robots = hex->workersInHex[WorkerType::Robot];
	workers.aliens = hex->workersInHex[WorkerType::Alien];
	workers.maxWorkers = hex->maxWorkers;

	return workers;
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
		UTexture2D* buildingIcon = building.Value.buildingIcon;

		buildings.Add(FBuildingDisplay{name, production, workers, buildTime, buildingIcon});
	}

	return buildings;
}

FBuildingDisplay ABasePlayerController::GetBuildingDisplay(ABuilding* building)
{
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	FBuildingDisplay attachmentDisplay;
	if (!hex->building) return attachmentDisplay;

	SpawnableBuildings selectedBuilding = building->buildingType;
	if (!spawner->buildingStats.Contains(selectedBuilding)) return attachmentDisplay;

	attachmentDisplay.name = spawner->buildingCosts[selectedBuilding].name;
	attachmentDisplay.productionCost = FText::AsNumber(spawner->buildingCosts[selectedBuilding].productionCost);
	attachmentDisplay.workerCost = FText::AsNumber(spawner->buildingCosts[selectedBuilding].workerCost);
	attachmentDisplay.buildTime = FText::AsNumber(spawner->buildingCosts[selectedBuilding].timeToBuild);
	attachmentDisplay.buildingIcon = spawner->buildingStats[selectedBuilding].buildingIcon;

	return attachmentDisplay;
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

TArray<FBuildingDisplay> ABasePlayerController::GetAttachmentDisplays()
{
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	TArray<FBuildingDisplay> buildings;
	if (!hex) return buildings;

	TArray<TerrainType> nonBuildableTerrain = UnitActions::GetNonBuildableTerrain();
	if (nonBuildableTerrain.Contains(hex->hexTerrain)) return buildings;

	for (auto building : spawner->attachmentCosts)
	{
		FText name = building.Value.name;
		FText production = FText::AsNumber(building.Value.productionCost);
		FText workers = FText::AsNumber(building.Value.workerCost);
		FText buildTime = FText::AsNumber(building.Value.timeToBuild);
		UTexture2D* buildingIcon = building.Value.buildingIcon;

		buildings.Add(FBuildingDisplay{ name, production, workers, buildTime, buildingIcon });
	}

	return buildings;
}

FBuildingDisplay ABasePlayerController::GetAttachmentDisplay(FText attachmentName)
{
	FBuildingDisplay display;

	for (auto attachment : spawner->attachmentCosts)
	{
		if (attachmentName.EqualTo(attachment.Value.name))
		{
			display.name = attachment.Value.name;
			display.productionCost = FText::AsNumber(attachment.Value.productionCost);
			display.workerCost = FText::AsNumber(attachment.Value.workerCost);
			display.buildTime = FText::AsNumber(attachment.Value.timeToBuild);
			display.buildingIcon = attachment.Value.buildingIcon;

			break;
		}
	}

	return display;
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
			PlayUIBuildingSound(buildings.Key);
			return;
		}
	}
}
void ABasePlayerController::SelectBuildingAttachment(FText attachmentName)
{
	for (auto attachment : spawner->attachmentCosts)
	{
		if (attachmentName.EqualTo(attachment.Value.name))
		{
			AOutpost* outpost = GetOutpost();
			if (outpost)
			{
				spawner->BuildAttachment(playerFaction, attachment.Key, outpost);
				PlayUIAttachmentSound(attachment.Key);
			}

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

			if (outpost)
			{
				outpost->CueTroopBuild(troop.Key);
				PlayUITroopSound(troop.Key);
			}

			return;
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

int ABasePlayerController::GetStarveLevel()
{
	return UnitActions::GetFactionStarveLevel(playerFaction);
}

int ABasePlayerController::GetPowerOutageLevel()
{
	return UnitActions::GetFactionPowerOutageLevel(playerFaction);
}

FResourceGainLoss ABasePlayerController::GetResourceRates()
{
	FResourceGainLoss result;

	TMap<StratResources, int> resourceGains = UnitActions::GetResourcesPerTick(playerFaction);
	TMap<StratResources, int> resourceLosses = UnitActions::GetResourceLosses(playerFaction);

	result.wealthGain = resourceGains[StratResources::Wealth];
	result.wealthLoss = resourceLosses[StratResources::Wealth];
	result.energyGain = resourceGains[StratResources::Energy];
	result.energyLoss = resourceLosses[StratResources::Energy];
	result.foodGain = resourceGains[StratResources::Food];
	result.foodLoss = resourceLosses[StratResources::Food];
	result.productionGain = resourceGains[StratResources::Production];
	result.productionLoss = resourceLosses[StratResources::Production];

	return result;
}

FWorkerSliders ABasePlayerController::SetWorkerCount(FWorkerSliders sliders)
{
	if (!selectedHex) return sliders;

	selectedHex->workersInHex[WorkerType::Human] += UnitActions::SetWorkers(Factions::Human, WorkerType::Human, FMath::RoundToInt(sliders.humanWorkers * selectedHex->maxWorkers), selectedHex);
	selectedHex->workersInHex[WorkerType::Robot] += UnitActions::SetWorkers(Factions::Human, WorkerType::Robot, FMath::RoundToInt(sliders.robotWorkers * selectedHex->maxWorkers), selectedHex);
	selectedHex->workersInHex[WorkerType::Alien] += UnitActions::SetWorkers(Factions::Human, WorkerType::Alien, FMath::RoundToInt(sliders.alienWorkers * selectedHex->maxWorkers), selectedHex);

	sliders.humanDisplay = selectedHex->workersInHex[WorkerType::Human];
	sliders.robotDisplay = selectedHex->workersInHex[WorkerType::Robot];
	sliders.alienDisplay = selectedHex->workersInHex[WorkerType::Alien];

	sliders.maxWorkers = selectedHex->maxWorkers;
	sliders.currWorkers = selectedHex->workersInHex[WorkerType::Human] + selectedHex->workersInHex[WorkerType::Robot] + selectedHex->workersInHex[WorkerType::Alien];

	sliders.availableHumans = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Human);
	sliders.availableRobots = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Robot);
	sliders.availableAliens = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Alien);

	return sliders;
}

FWorkerSliders ABasePlayerController::SetAttachmentWorkerCount(FWorkerSliders sliders, FText attachmentName)
{
	if (!selectedHex) return sliders;

	BuildingAttachments attachmentType = BuildingAttachments::Storage;
	for (auto& attachment : spawner->attachmentCosts)
	{
		if (attachmentName.EqualTo(attachment.Value.name))
		{
			attachmentType = attachment.Key;
			break;
		}
	}

	if (AOutpost* outpost = GetOutpost())
	{
		UBuildingAttachment* attachment = outpost->GetAttachment(attachmentType);
		if (!attachment) return sliders;

		attachment->workersInAttachment[WorkerType::Human] += UnitActions::SetWorkers(Factions::Human, WorkerType::Human, FMath::RoundToInt(sliders.humanWorkers * attachment->maxWorkers), outpost, attachmentType);
		attachment->workersInAttachment[WorkerType::Robot] += UnitActions::SetWorkers(Factions::Human, WorkerType::Robot, FMath::RoundToInt(sliders.robotWorkers * attachment->maxWorkers), outpost, attachmentType);
		attachment->workersInAttachment[WorkerType::Alien] += UnitActions::SetWorkers(Factions::Human, WorkerType::Alien, FMath::RoundToInt(sliders.alienWorkers * attachment->maxWorkers), outpost, attachmentType);

		sliders.humanDisplay = attachment->workersInAttachment[WorkerType::Human];
		sliders.robotDisplay = attachment->workersInAttachment[WorkerType::Robot];
		sliders.alienDisplay = attachment->workersInAttachment[WorkerType::Alien];

		sliders.maxWorkers = attachment->maxWorkers;
		sliders.currWorkers = attachment->workersInAttachment[WorkerType::Human] + attachment->workersInAttachment[WorkerType::Robot] + attachment->workersInAttachment[WorkerType::Alien];

		sliders.availableHumans = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Human);
		sliders.availableRobots = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Robot);
		sliders.availableAliens = UnitActions::GetAvailableWorkerType(playerFaction, WorkerType::Alien);
	}

	return sliders;
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

bool ABasePlayerController::OutpostCanBuildTroops()
{
	AOutpost* outpost = GetOutpost();

	if (!outpost) return false;
	if (outpost->unitStats->faction != Factions::Human) return false;

	return outpost->BuildingAttachmentIsActive(BuildingAttachments::RobotFactory);
}

bool ABasePlayerController::OutpostCanStoreTroops()
{
	AOutpost* outpost = GetOutpost();

	if (!outpost) return false;
	if (outpost->unitStats->faction != Factions::Human) return false;

	return outpost->BuildingAttachmentIsActive(BuildingAttachments::RobotBarracks);
}

bool ABasePlayerController::AttachmentIsActive(FText attachmentName)
{
	for (auto attachment : spawner->attachmentCosts)
	{
		if (attachmentName.EqualTo(attachment.Value.name))
		{
			AOutpost* outpost = GetOutpost();
			if (outpost)
			{
				return outpost->GetAttachment(attachment.Key)->AttachmentIsActive();
			}
		}
	}

	return false;
}

bool ABasePlayerController::ToggleFarmlandYield(bool produceFood)
{
	FBuildingOnHex buildingOnHex = GetBuildingOnHex();
	if (buildingOnHex.farmland)
	{
		buildingOnHex.farmland->ToggleResourcesProduced(produceFood);
	}

	return produceFood;
}

FBuildingOnHex ABasePlayerController::GetBuildingOnHex()
{
	FBuildingOnHex hexBuilding = FBuildingOnHex{ SpawnableBuildings::None, nullptr, nullptr, nullptr, nullptr, nullptr };

	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return hexBuilding;

	ABuilding* building = hex->building;
	if (!building) return hexBuilding;

	hexBuilding.baseBuilding = building;

	if (AFarmland* farmland = Cast<AFarmland>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::Farmland;
		hexBuilding.farmland = farmland;
		return hexBuilding;
	}

	if (APowerPlant* powerplant = Cast<APowerPlant>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::PowerPlant;
		hexBuilding.powerplant = powerplant;
		return hexBuilding;
	}

	if (AMiningStation* miningStation = Cast<AMiningStation>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::MiningStation;
		hexBuilding.miningStation = miningStation;
		return hexBuilding;
	}

	if (AOutpost* outpost = Cast<AOutpost>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::Outpost;
		hexBuilding.outpost = outpost;
		return hexBuilding;
	}

	hexBuilding.buildingType = SpawnableBuildings::None;
	return hexBuilding;
}

void ABasePlayerController::CheckForActionStates()
{
	int actionStatesActive = 0;

	for (auto& state : actionStates)
	{
		if (state.Value != nullptr) actionStatesActive++;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Green, FString::Printf(TEXT("%d states active"), actionStatesActive));
}

void ABasePlayerController::ResourceCheats(int resourceToChange, int val)
{
	if (0 < resourceToChange && resourceToChange < 5)
	{
		UnitActions::SetFactionResources(playerFaction, StratResources(resourceToChange), val);
	}
}

FBuildingTTInfo ABasePlayerController::GetBuildingTTDisplay(FText buildingName)
{
	FBuildingTTInfo buildingInfo;
	FBuildingStats buildingStats;

	bool statsFound = false;

	for (auto& building : spawner->buildingStats)
	{
		if (buildingName.EqualTo(building.Value.name))
		{
			buildingStats = building.Value;
			statsFound = true;
			break;
		}
	}

	if (!statsFound)
	{
		for (auto& building : spawner->attachmentStats)
		{
			if (buildingName.EqualTo(building.Value.name))
			{
				buildingStats = building.Value;
				statsFound = true;
				break;
			}
		}
	}
	if (!statsFound) return buildingInfo;

	buildingInfo.titleTT = buildingStats.name;
	buildingInfo.descTT = buildingStats.description;

	buildingInfo.energyMod = FText::AsNumber(buildingStats.energyYield);
	buildingInfo.foodMod = FText::AsNumber(buildingStats.foodYield);
	buildingInfo.prodMod = FText::AsNumber(buildingStats.productionYield);
	buildingInfo.wealthMod = FText::AsNumber(buildingStats.wealthYield);

	buildingInfo.resourceStorageMod = FText::AsNumber(buildingStats.resourceCapIncrease);
	buildingInfo.robotStorageMod = FText::AsNumber(buildingStats.robotStorage);

	buildingInfo.diploMod = FText::AsNumber(buildingStats.diplomacy);
	buildingInfo.tradeMod = FText::AsNumber(buildingStats.trade);

	buildingInfo.siegeDamage = FText::AsNumber(buildingStats.siegeDamage);
	buildingInfo.siegeHP = FText::AsNumber(buildingStats.HP);

	buildingInfo.unrestMod = FText::AsNumber(buildingStats.unrestMod);
	buildingInfo.energyUpkeepCost = FText::AsNumber(buildingStats.energyUpkeepCost);
	buildingInfo.maxWorkers = FText::AsNumber(buildingStats.maxWorkers);

	return buildingInfo;
}

FTroopTTInfo ABasePlayerController::GetTroopTTDisplay(FText troopName)
{
	FTroopTTInfo info = FTroopTTInfo{};
	FTroopStats troopStats;

	for (auto troop : spawner->troopStats)
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


