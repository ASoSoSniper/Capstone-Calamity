// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/FactionManager.h"
#include "CapstoneProjectGameModeBase.h"
#include "Types/DisplayInfo.h"

AFactionManager* AFactionManager::_instance = nullptr;

// Sets default values
AFactionManager::AFactionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	humanStartResources.Add(EStratResources::Food, 300);
	humanStartResources.Add(EStratResources::Production, 300);
	humanStartResources.Add(EStratResources::Energy, 300);
	humanStartResources.Add(EStratResources::Wealth, 0);
	humanWorkers.Add(WorkerType::Organic, FWorkerStats{0, 100, 0, { {EStratResources::Food, 1} }, { {EStratResources::Food, 1} } });
	humanWorkers.Add(WorkerType::Robot, FWorkerStats{0, 10, 0, {}, { {EStratResources::Energy, 1} } });

	alienStartResources.Add(EStratResources::Food, 300);
	alienStartResources.Add(EStratResources::Production, 300);
	alienStartResources.Add(EStratResources::Energy, 300);
	alienStartResources.Add(EStratResources::Wealth, 0);
	alienWorkers.Add(WorkerType::Organic, FWorkerStats{ 0, 1000, 0, { {EStratResources::Food, 1} }, { {EStratResources::Food, 1} } });
	alienWorkers.Add(WorkerType::Robot, FWorkerStats{ 0, 0, 0, {}, { {EStratResources::Energy, 1} } });

	_factionColors.Add(EFactions::None, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/Unclaimed.Unclaimed")));
	_factionColors.Add(EFactions::Human, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/HumanOwned.HumanOwned")));
	_factionColors.Add(EFactions::Alien1, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned01.AlienOwned01")));
	_factionColors.Add(EFactions::Alien2, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned02.AlienOwned02")));
	_factionColors.Add(EFactions::Alien3, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned03.AlienOwned03")));
	_factionColors.Add(EFactions::Alien4, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned04.AlienOwned04")));
	_factionColors.Add(EFactions::Alien5, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned05.AlienOwned05")));
	_factionColors.Add(EFactions::Alien6, LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileUndertones/AlienOwned06.AlienOwned06")));
}

// Called when the game starts or when spawned
void AFactionManager::BeginPlay()
{
	Super::BeginPlay();
	_instance = this;

	ACapstoneProjectGameModeBase::onDateTick.AddDynamic(this, &AFactionManager::HandleDateTick);

	//Create initial player faction and assign it to the player controller
	CreateNewFaction();

	//Create alien factions, quantity determined by variable
	for (int i = 0; i < _alienFactionQuantity; ++i)
	{
		CreateNewFaction();
	}

	for (auto& currentFaction : _activeFactions)
	{
		currentFaction.Value->FindActiveFactions();
		currentFaction.Value->factionColor = _factionColors[currentFaction.Key];
	}
}

// Called every frame
void AFactionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

bool AFactionManager::Initialized()
{
	return _instance != nullptr;
}

UFaction* AFactionManager::GetFactionObject(EFactions faction)
{
	if (!_instance->_activeFactions.Contains(faction)) return nullptr;

	return _instance->_activeFactions[faction];
}

const TMap<EFactions, UFaction*>& AFactionManager::GetAllFactions()
{
	return _instance->_activeFactions;
}

const FFactionDisplay* AFactionManager::GetFactionDisplayPreset(EFactions faction)
{
	if (!_instance->_factionDisplayPresets.Contains(faction)) return nullptr;

	return &_instance->_factionDisplayPresets[faction];
}

UMaterialInterface* AFactionManager::GetFactionColor(EFactions faction)
{
	if (!_instance->_factionColors.Contains(faction)) return nullptr;

	return _instance->_factionColors[faction];
}

bool AFactionManager::FactionAIEnabled()
{
	return _instance->enableFactionAI;
}

int32 AFactionManager::GetStartingResourceCap()
{
	return _instance->startingResourceCap;
}

int32 AFactionManager::GetTicksTillHarvest()
{
	return _instance->ticksTillHarvest;
}

int32 AFactionManager::GetPopDeathsPerFoodMissing()
{
	return _instance->popDeathsPerFoodMissing;
}

int32 AFactionManager::GetPopDeathsPerPowerMissing()
{
	return _instance->popDeathsPerPowerMissing;
}

int32 AFactionManager::GetDaysTillStarvation()
{
	return _instance->daysTillStarve;
}

int32 AFactionManager::GetDaysTillPowerOutage()
{
	return _instance->daysTillPowerOutage;
}

EFactions AFactionManager::CreateNewFaction()
{
	//Advance in the Factions enum, element 0 is None
	++_factionCount;

	//Get the selected faction in the Factions enum
	EFactions selectedFaction = static_cast<EFactions>(_factionCount);

	//Create a new Faction instance
	UFaction* newFaction = NewObject<UFaction>();
	newFaction->AddToRoot();

	AFactionController* factionController = selectedFaction == EFactions::Human ? nullptr :
		GetWorld()->SpawnActor<AFactionController>(factionControllerPrefab ? factionControllerPrefab : AFactionController::StaticClass());

	// Initialize faction instance with starting values
	const TMap<EStratResources, int>& startResources = selectedFaction == EFactions::Human ? humanStartResources : alienStartResources;
	const TMap<WorkerType, FWorkerStats>& startWorkers = selectedFaction == EFactions::Human ? humanWorkers : alienWorkers;

	newFaction->SetFaction(selectedFaction, startResources, startWorkers);

	if (factionController)
	{
		newFaction->SetFactionController(factionController);
		factionController->SetFaction(newFaction);
	}

	//In the global faction dictionary, add the selected faction as a new key and the Faction instance as its value
	_activeFactions.Add(selectedFaction, newFaction);

	//Return the selected faction
	return selectedFaction;
}

void AFactionManager::HandleDateTick(const FDateTickUpdate& date)
{
	_currTicksTillHarvest++;
	if (_currTicksTillHarvest >= ticksTillHarvest)
	{
		for (const auto& faction : _activeFactions)
		{
			faction.Value->Harvest();
		}
		_currTicksTillHarvest = 0;
	}
}