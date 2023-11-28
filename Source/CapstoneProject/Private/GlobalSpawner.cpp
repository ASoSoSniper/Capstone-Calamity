// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalSpawner.h"
#include "BaseHex.h"
#include "HexNav.h"
#include "Building.h"
#include "Troop.h"
#include "MergedArmy.h"
#include "MiningStation.h"
#include "Farmland.h"
#include "PowerPlant.h"
#include "Outpost.h"
#include "MaterialStorage.h"
#include "BattleObject.h"

// Sets default values
AGlobalSpawner::AGlobalSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	buildingCosts.Add(SpawnableBuildings::MiningStation, FBuildingCost{ 200, 30 });
	buildingCosts.Add(SpawnableBuildings::Farmland, FBuildingCost{ 100, 30 });
	buildingCosts.Add(SpawnableBuildings::PowerPlant, FBuildingCost{ 200, 30 });
	buildingCosts.Add(SpawnableBuildings::Outpost, FBuildingCost{ 0, 0 });

	attachmentCosts.Add(BuildingAttachments::Storage, FBuildingCost{ 100, 15 });
	attachmentCosts.Add(BuildingAttachments::DefenseStation, FBuildingCost{ 100, 20 });
	attachmentCosts.Add(BuildingAttachments::RobotFactory, FBuildingCost{ 100, 30 });
	attachmentCosts.Add(BuildingAttachments::RobotBarracks, FBuildingCost{ 100, 15 });

	attachmentCosts.Add(BuildingAttachments::TradeOutpost, FBuildingCost{ 50, 5 });
	attachmentCosts.Add(BuildingAttachments::Embassy, FBuildingCost{ 50, 5 });
	attachmentCosts.Add(BuildingAttachments::PoliceStation, FBuildingCost{ 100, 40 });
}

// Called when the game starts or when spawned
void AGlobalSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (!troopPrefab) troopPrefab = ATroop::StaticClass();
	if (!mergedArmyPrefab) mergedArmyPrefab = AMergedArmy::StaticClass();
	if (!miningStationPrefab) miningStationPrefab = AMiningStation::StaticClass();
	if (!farmlandPrefab) farmlandPrefab = AFarmland::StaticClass();
	if (!powerPlantPrefab) powerPlantPrefab = APowerPlant::StaticClass();
	if (!outpostPrefab) outpostPrefab = AOutpost::StaticClass();
	if (!battlePrefab) battlePrefab = ABattleObject::StaticClass();
	if (!materialStoragePrefab) materialStoragePrefab = AMaterialStorage::StaticClass();
}

// Called every frame
void AGlobalSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UClass* AGlobalSpawner::DetermineBuildingType(SpawnableBuildings building)
{
	switch (building)
	{
	case SpawnableBuildings::MiningStation:
		return miningStationPrefab;
	case SpawnableBuildings::Farmland:
		return farmlandPrefab;
	case SpawnableBuildings::PowerPlant:
		return powerPlantPrefab;
	case SpawnableBuildings::Outpost:
		return outpostPrefab;
	default:
		return nullptr;
	}
}

UClass* AGlobalSpawner::DetermineUnitType(SpawnableUnits unit)
{
	return troopPrefab;
}

void AGlobalSpawner::MergeArmies(ATroop* seeker, ATroop* target, ABaseHex* hex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Armies merged!"));
	int armies = 0;

	AMergedArmy* testSeeker = Cast<AMergedArmy>(seeker);
	if (testSeeker) armies += 1;
	AMergedArmy* testTarget = Cast<AMergedArmy>(target);
	if (testTarget) armies += 1;

	FActorSpawnParameters params;
	AMergedArmy* mergedArmy;

	switch (armies)
	{
		//If neither unit is an army, create a new army that consumes both units
	case 0:
		mergedArmy = GetWorld()->SpawnActor<AMergedArmy>(mergedArmyPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);

		hex->troopsInHex.Remove(seeker);
		hex->troopsInHex.Remove(target);

		mergedArmy->unitStats->faction = seeker->unitStats->faction;
		mergedArmy->ConsumeUnit(seeker);
		mergedArmy->ConsumeUnit(target);
		break;

		//If one unit is an army, determine which unit is that army and merge the other into it
	case 1:
		AMergedArmy * army;
		ATroop* unit;
		if (testSeeker)
		{
			army = testSeeker;
			unit = target;
		}
		else
		{
			army = testTarget;
			unit = seeker;
		}

		hex->troopsInHex.Remove(unit);

		army->ConsumeUnit(unit);
		break;

		//If both units are armies, let the seeker be the dominant army and merge the other army into it
	case 2:
		hex->troopsInHex.Remove(testTarget);

		testSeeker->ConsumeArmy(testTarget);
		break;
	}
}

void AGlobalSpawner::SpawnBuilding(Factions faction, SpawnableBuildings building, ABaseHex* hex)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Spawn building"));
	if (hex->building == nullptr)
	{
		
		FActorSpawnParameters params;
		UClass* prefab = DetermineBuildingType(building);

		if (!prefab) return;

		bool canAfford = false;

		TMap<StratResources, int> resourceCosts = TMap<StratResources, int>();
		TMap<WorkerType, int> workerCosts = TMap<WorkerType, int>();
		if (buildingCosts.Contains(building))
		{
			TMap<StratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);
			TMap<WorkerType, int> workers = UnitActions::GetFactionWorkers(faction);

			if (resources[StratResources::Production] >= buildingCosts[building].productionCost && workers[WorkerType::Robot] > buildingCosts[building].workerCost)
			{
				canAfford = true;
			}

			resourceCosts.Add(StratResources::Production, buildingCosts[building].productionCost);
			workerCosts.Add(WorkerType::Robot, buildingCosts[building].workerCost);
		}

		if (canAfford)
		{
			UnitActions::ConsumeSpentResources(faction, resourceCosts, workerCosts, hex);
			ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(prefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
			UnitActions::AssignFaction(faction, newBuilding);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Cannot afford building"));
		}
	}
	else GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Hex already occupied"));
}

ATroop* AGlobalSpawner::SpawnTroop(ABaseHex* hex, UnitActions::UnitData data, float parentHealthPercent)
{
	FActorSpawnParameters params;
	if (!troopPrefab) return nullptr;

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);

	float result = (float)data.currentHP * parentHealthPercent;
	if (result < 1.f) result = 1.f;
	data.currentHP = result;

	newTroop->InputUnitStats(data);

	return newTroop;
}

AMergedArmy* AGlobalSpawner::SpawnArmy(ABaseHex* hex, TArray<UnitActions::UnitData> groupData, float parentHealthPercent)
{
	FActorSpawnParameters params;
	if (!mergedArmyPrefab) return nullptr;

	AMergedArmy* newTroop = GetWorld()->SpawnActor<AMergedArmy>(mergedArmyPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
	
	for (int i = 0; i < groupData.Num(); ++i)
	{
		float result = (float)groupData[i].currentHP * parentHealthPercent;
		if (result < 1.f) result = 1.f;
		groupData[i].currentHP = result;
	}

	newTroop->ConsumeData(groupData);

	return newTroop;
}

ABattleObject* AGlobalSpawner::SpawnBattle(ABaseHex* hex)
{
	FActorSpawnParameters params;
	ABattleObject* battle = GetWorld()->SpawnActor<ABattleObject>(battlePrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0.f, 0.f, 0.f), params);
	battle->hexNav->currentHex = hex;
	battle->spawner = this;
	hex->battleInProgress = true;
	battle->Start();

	return battle;
}

