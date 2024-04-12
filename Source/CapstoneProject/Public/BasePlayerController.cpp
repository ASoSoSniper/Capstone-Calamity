// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "HexNav.h"
#include "UnitActions.h"
#include "MergedArmy.h"
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

	currentActionState = ActionStates::None;

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
	if (!sound) return;

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

FTroopArmyDisplay ABasePlayerController::GetBattleUnit(int group)
{
	FTroopArmyDisplay unit = FTroopArmyDisplay{ FText::FromString(TEXT("Goofy ah")), 0, 0, 0, 0, 0, FText::FromString(TEXT("0")), nullptr, 0,0,0,0,0,0 };

	ABattleObject* battleObject = Cast<ABattleObject>(selectedWorldObject);
	if (!battleObject) return unit;

	unit.hexIcon = battleObject->hex->GetDisplayInfo().icon;
	unit.dieRoll = group == 0 ? battleObject->group1Die : battleObject->group2Die;
	unit.name = FText::FromString(TEXT("Goofy ah"));
	unit.defenderBonus = FText::AsNumber(battleObject->hex->defenderBonus);

	if (battleObject->currentBattle.Group1.IsEmpty() || battleObject->currentBattle.Group2.IsEmpty()) return unit;

	Factions selectedFaction = group == 0 ? battleObject->currentBattle.Group1[0] : battleObject->currentBattle.Group2[0];

	unit.HP = battleObject->armies[selectedFaction].currentHP;
	unit.maxHP = battleObject->armies[selectedFaction].maxHP;
	unit.morale = battleObject->armies[selectedFaction].currentMorale;
	unit.maxMorale = battleObject->armies[selectedFaction].maxMorale;

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

FArmyDisplay ABasePlayerController::DisplaySelectedUnit()
{
	FArmyDisplay display;
	AActor* actor = GetActionStateSelection();

	if (!actor) return display;
	ATroop* troop = Cast<ATroop>(actor);

	if (!troop) return display;

	FTroopStats stats;
	FTroopCost costs;
	if (spawner->troopStats.Contains(troop->unitStats->unitType))
	{
		stats = spawner->troopStats[troop->unitStats->unitType];
	}
	if (spawner->troopCosts.Contains(troop->unitStats->unitType))
	{
		costs = spawner->troopCosts[troop->unitStats->unitType];
	}

	if (troop->unitStats->unitType != UnitTypes::Army)
	{
		display.name = stats.title;
		display.icon = costs.icon;
	}	
	else
	{
		UnitTypes chosenType = UnitActions::GetLargestUnitQuantity(troop);

		display.name = spawner->troopStats[chosenType].title;
		display.icon = spawner->troopCosts[chosenType].icon;
	}

	display.HP = troop->unitStats->currentHP;
	display.HPMax = troop->unitStats->maxHP;
	display.morale = troop->unitStats->currentMorale;
	display.moraleMax = troop->unitStats->maxMorale;

	display.damage = troop->unitStats->damage;
	display.buildingDamage = troop->unitStats->siegePower;

	display.speed = troop->unitStats->speed;
	display.energyCost = troop->unitStats->energyUpkeep;

	return display;
}

FArmyMenuInfo ABasePlayerController::DisplayArmyMenu()
{
	FArmyMenuInfo display{ FText::FromString("Army Name"), nullptr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	AActor* actor = GetActionStateSelection();

	if (!actor) return display;
	ATroop* army = Cast<ATroop>(actor);
	if (!army) return display;

	if (army->unitStats->unitType != UnitTypes::Army)
	{
		display.name = spawner->troopStats[army->unitStats->unitType].title;
		display.icon = spawner->troopCosts[army->unitStats->unitType].icon;
	}
	else
	{
		UnitTypes chosenType = UnitActions::GetLargestUnitQuantity(army);

		display.name = spawner->troopStats[chosenType].title;
		display.icon = spawner->troopCosts[chosenType].icon;
	}

	display.HP = army->unitStats->currentHP;
	display.HPMax = army->unitStats->maxHP;
	display.morale = army->unitStats->currentMorale;
	display.moraleMax = army->unitStats->maxMorale;

	display.damage = army->unitStats->damage;
	display.buildingDamage = army->unitStats->siegePower;

	display.speed = army->unitStats->speed;
	display.energyUpkeepCost = army->unitStats->energyUpkeep;

	display.vision = 0;

	TMap<UnitTypes, FUnitComposition> units = UnitActions::GetArmyComposition(army);

	if (army->unitStats->unitType == UnitTypes::Army)
	{
		int totalUnits = 0;
		for (auto& unit : units)
		{
			totalUnits += unit.Value.quantity;

			display.attackvsInfantry += unit.Value.quantity * spawner->troopStats[unit.Key].attackvsInfantry;
			display.attackvsCavalry += unit.Value.quantity * spawner->troopStats[unit.Key].attackvsCavalry;
			display.attackvsScout += unit.Value.quantity * spawner->troopStats[unit.Key].attackvsScout;
			display.attackvsRanged += unit.Value.quantity * spawner->troopStats[unit.Key].attackvsRanged;
			display.attackvsShielder += unit.Value.quantity * spawner->troopStats[unit.Key].attackvsShielder;
			display.attackvsSettler += unit.Value.quantity * spawner->troopStats[unit.Key].attackvsSettler;

			display.defendvsInfantry += unit.Value.quantity * spawner->troopStats[unit.Key].defendvsInfantry;
			display.defendvsCavalry += unit.Value.quantity * spawner->troopStats[unit.Key].defendvsCavalry;
			display.defendvsScout += unit.Value.quantity * spawner->troopStats[unit.Key].defendvsScout;
			display.defendvsRanged += unit.Value.quantity * spawner->troopStats[unit.Key].defendvsRanged;
			display.defendvsShielder += unit.Value.quantity * spawner->troopStats[unit.Key].defendvsShielder;
			display.defendvsSettler += unit.Value.quantity * spawner->troopStats[unit.Key].defendvsSettler;
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
		display.attackvsInfantry = spawner->troopStats[army->unitStats->unitType].attackvsInfantry;
		display.attackvsCavalry = spawner->troopStats[army->unitStats->unitType].attackvsCavalry;
		display.attackvsScout = spawner->troopStats[army->unitStats->unitType].attackvsScout;
		display.attackvsRanged = spawner->troopStats[army->unitStats->unitType].attackvsRanged;
		display.attackvsShielder = spawner->troopStats[army->unitStats->unitType].attackvsShielder;
		display.attackvsSettler = spawner->troopStats[army->unitStats->unitType].attackvsSettler;

		display.defendvsInfantry = spawner->troopStats[army->unitStats->unitType].defendvsInfantry;
		display.defendvsCavalry = spawner->troopStats[army->unitStats->unitType].defendvsCavalry;
		display.defendvsScout = spawner->troopStats[army->unitStats->unitType].defendvsScout;
		display.defendvsRanged = spawner->troopStats[army->unitStats->unitType].defendvsRanged;
		display.defendvsShielder = spawner->troopStats[army->unitStats->unitType].defendvsShielder;
		display.defendvsSettler = spawner->troopStats[army->unitStats->unitType].defendvsSettler;

		switch (army->unitStats->unitType)
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

void ABasePlayerController::SplitArmyInHalf()
{
	AActor* actor = GetActionStateSelection();
	if (!actor) return;

	ATroop* troop = Cast<ATroop>(actor);
	if (!troop) return;

	if (troop->unitStats->unitType != UnitTypes::Army) return;
	AMergedArmy* army = Cast<AMergedArmy>(troop);

	if (army) army->SplitInHalf();
}

FAttachmentBuildProgress ABasePlayerController::GetAttachmentBuildProgress(BuildingAttachments attachment)
{
	FAttachmentBuildProgress buildProgress = FAttachmentBuildProgress();
	AOutpost* outpost = GetOutpost();
	if (!outpost) return buildProgress;

	UBuildingAttachment* component = outpost->GetAttachment(attachment);
	
	buildProgress.currentProgress = component->currBuildTime;
	buildProgress.buildTime = component->buildTime;
	buildProgress.isBuilding = component->buildState == BuildStates::Building;

	return buildProgress;
}

FAttachmentTTBuildInfo ABasePlayerController::GetAttachmentBuildInfo(BuildingAttachments attachment)
{
	FAttachmentTTBuildInfo info;
	if (!spawner->attachmentCosts.Contains(attachment) || !spawner->attachmentStats.Contains(attachment)) return info;

	FBuildingCost cost = spawner->attachmentCosts[attachment];
	FBuildingStats stat = spawner->attachmentStats[attachment];

	info.title = cost.name;
	info.description = stat.description;

	info.productionCost = cost.productionCost;
	info.workerCost = cost.workerCost;
	info.buildTime = cost.timeToBuild;

	return info;
}

FMaterialStorageStats ABasePlayerController::GetMaterialStorageStats()
{
	FMaterialStorageStats storage;
	if (!spawner->attachmentStats.Contains(BuildingAttachments::Storage)) return storage;

	FBuildingStats stats = spawner->attachmentStats[BuildingAttachments::Storage];

	storage.title = stats.name;
	storage.description = stats.description;
	storage.energyCost = stats.energyUpkeepCost;

	storage.resourceIncrease = stats.resourceCapIncrease;

	return storage;
}

FRobotFactoryStats ABasePlayerController::GetRobotFactoryStats()
{
	FRobotFactoryStats factory;
	if (!spawner->attachmentStats.Contains(BuildingAttachments::RobotFactory)) return factory;

	FBuildingStats stats = spawner->attachmentStats[BuildingAttachments::RobotFactory];

	factory.title = stats.name;
	factory.description = stats.description;
	factory.energyCost = stats.energyUpkeepCost;

	return factory;
}

FRobotStorageStats ABasePlayerController::GetRobotStorageStats()
{
	FRobotStorageStats robotStorage;
	if (!spawner->attachmentStats.Contains(BuildingAttachments::RobotBarracks)) return robotStorage;

	FBuildingStats stats = spawner->attachmentStats[BuildingAttachments::RobotBarracks];

	robotStorage.title = stats.name;
	robotStorage.description = stats.description;
	robotStorage.energyCost = stats.energyUpkeepCost;

	robotStorage.maxRobotStorage = stats.robotStorage;

	return robotStorage;
}

FDefenseStationStats ABasePlayerController::GetDefenseStationStats()
{
	FDefenseStationStats defense;
	if (!spawner->attachmentStats.Contains(BuildingAttachments::DefenseStation)) return defense;

	FBuildingStats stats = spawner->attachmentStats[BuildingAttachments::DefenseStation];

	defense.title = stats.name;
	defense.description = stats.description;
	defense.energyCost = stats.energyUpkeepCost;

	defense.HP = stats.HP;
	defense.damage = stats.siegeDamage;

	return defense;
}

bool ABasePlayerController::CanBuildOnHex()
{
	if (!selectedHex) return false;

	if (selectedHex->hexTerrain == TerrainType::Mountains || 
		selectedHex->hexTerrain == TerrainType::Border || 
		selectedHex->hexTerrain == TerrainType::Jungle) return false;

	return true;
}

bool ABasePlayerController::CanPutWorkersOnHex()
{
	if (!selectedHex) return false;

	if (selectedHex->hexTerrain == TerrainType::Mountains ||
		selectedHex->hexTerrain == TerrainType::Border || 
		selectedHex->hexTerrain == TerrainType::AlienCity ||
		selectedHex->hexTerrain == TerrainType::TheRock) return false;

	return true;
}

bool ABasePlayerController::HexIsDiscovered()
{
	if (!selectedHex) return false;

	if (!selectedHex->visibility->discoveredByPlayer) return false;

	return true;
}

bool ABasePlayerController::IsHumanControlled(AActor* object)
{
	UnitActions::SelectionIdentity identity = UnitActions::DetermineObjectType(object);

	switch (identity.type)
	{
	case ObjectTypes::MoveAI:
		return identity.moveAI->unitStats->faction == Factions::Human;

	case ObjectTypes::Building:
		return (identity.building->unitStats->faction == Factions::Human && identity.building->siegingFaction == Factions::None) ||
			(identity.building->unitStats->faction != Factions::Human && identity.building->siegingFaction == Factions::Human);

	case ObjectTypes::Hex:
		return identity.hex->hexOwner == Factions::Human;
	}

	return false;
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

void ABasePlayerController::PlayUIAttachmentSound(BuildingAttachments attachmentType)
{
	if (!UIAttachmentSounds.Contains(attachmentType)) return;

	playerCamera->audioComponent->SetSound(UIAttachmentSounds[attachmentType]);
	playerCamera->audioComponent->Play();
}

void ABasePlayerController::PlayUIHexSound(TerrainType hexType, ABaseHex* hex)
{
	if (hex)
	{
		if (hex->building)
		{
			PlayUIBuildingSound(hex->building->buildingType);
			return;
		}
	}

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

ActionStates ABasePlayerController::GetActionState()
{
	return currentActionState;
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

FWorkersInHex ABasePlayerController::GetWorkersInAttachment(BuildingAttachments attachment)
{
	FWorkersInHex workers = FWorkersInHex{ 0,0,0 };

	if (AOutpost* outpost = GetOutpost())
	{
		UBuildingAttachment* selectedAttachment = outpost->GetAttachment(attachment);
		if (!selectedAttachment) return workers;

		workers.humans = selectedAttachment->workersInAttachment[WorkerType::Human];
		workers.robots = selectedAttachment->workersInAttachment[WorkerType::Robot];
		workers.aliens = selectedAttachment->workersInAttachment[WorkerType::Alien];
		workers.maxWorkers = selectedAttachment->maxWorkers;
	}

	return workers;
}

TArray<FBuildingDisplay> ABasePlayerController::GetBuildingDisplays()
{
	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	TArray<FBuildingDisplay> buildings;
	if (!hex) return buildings;

	//TArray<TerrainType> nonBuildableTerrain = UnitActions::GetNonBuildableTerrain();
	//if (nonBuildableTerrain.Contains(hex->hexTerrain)) return buildings;

	for (auto& building : spawner->buildingCosts)
	{
		if (building.Key == SpawnableBuildings::Capitol && firstBuildPerformed) continue;
		if (building.Key != SpawnableBuildings::Capitol && !firstBuildPerformed) continue;

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

	FBuildingCost costs;
	FBuildingStats stats;
	if (spawner->buildingCosts.Contains(selectedBuilding))
	{
		costs = spawner->buildingCosts[selectedBuilding];
	}

	if (!spawner->buildingStats.Contains(selectedBuilding)) return attachmentDisplay;

	attachmentDisplay.name = spawner->buildingStats[selectedBuilding].name;
	attachmentDisplay.productionCost = FText::AsNumber(costs.productionCost);
	attachmentDisplay.workerCost = FText::AsNumber(costs.workerCost);
	attachmentDisplay.buildTime = FText::AsNumber(costs.timeToBuild);
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

bool ABasePlayerController::IsInBuildMode()
{
	return actionStates[currentActionState]->subSelect == 1;
}

//FOR BLUEPRINT: Triggered when a building is selected from the UI, identifies which building was selected and builds it accordingly
void ABasePlayerController::SelectBuilding(FText buildingName)
{
	for (auto& buildings : spawner->buildingCosts)
	{
		if (buildingName.EqualTo(buildings.Value.name))
		{
			if (buildings.Key == SpawnableBuildings::Capitol)
			{
				if (selectedHex->hexTerrain == TerrainType::Ship) firstBuildPerformed = true;
				else return;
			}

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
	for (auto& troop : spawner->troopCosts)
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

FWorkerSliders ABasePlayerController::SetAttachmentWorkerCount(FWorkerSliders sliders, BuildingAttachments attachmentType)
{
	if (!selectedHex) return sliders;

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
	FBuildingOnHex hexBuilding = FBuildingOnHex{ SpawnableBuildings::None, FText{}, nullptr, nullptr, nullptr, nullptr, nullptr};

	ABaseHex* hex = Cast<ABaseHex>(selectedWorldObject);
	if (!hex) return hexBuilding;

	ABuilding* building = hex->building;
	if (!building) return hexBuilding;

	hexBuilding.baseBuilding = building;

	if (AFarmland* farmland = Cast<AFarmland>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::Farmland;
		hexBuilding.farmland = farmland;
		hexBuilding.name = spawner->buildingCosts[SpawnableBuildings::Farmland].name;
		return hexBuilding;
	}

	if (APowerPlant* powerplant = Cast<APowerPlant>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::PowerPlant;
		hexBuilding.powerplant = powerplant;
		hexBuilding.name = spawner->buildingCosts[SpawnableBuildings::PowerPlant].name;
		return hexBuilding;
	}

	if (AMiningStation* miningStation = Cast<AMiningStation>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::MiningStation;
		hexBuilding.miningStation = miningStation;
		hexBuilding.name = spawner->buildingCosts[SpawnableBuildings::MiningStation].name;
		return hexBuilding;
	}

	if (ACapitalHub* hub = Cast<ACapitalHub>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::Capitol;
		hexBuilding.hub = hub;
		hexBuilding.outpost = hub;
		hexBuilding.name = spawner->buildingCosts[SpawnableBuildings::Capitol].name;
		return hexBuilding;
	}

	if (AOutpost* outpost = Cast<AOutpost>(hex->building))
	{
		hexBuilding.buildingType = SpawnableBuildings::Outpost;
		hexBuilding.outpost = outpost;
		hexBuilding.name = spawner->buildingCosts[SpawnableBuildings::Outpost].name;
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

	for (auto& troop : spawner->troopStats)
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


