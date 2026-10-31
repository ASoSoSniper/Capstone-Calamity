// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalSpawner.h"
#include "BasePlayerController.h"
#include "BaseHex.h"
#include "HexNav.h"
#include "Faction.h"
#include "Building.h"
#include "Troop.h"
#include "Settler.h"
#include "MergedArmy.h"
#include "MiningStation.h"
#include "Farmland.h"
#include "PowerPlant.h"
#include "MaterialStorage.h"
#include "TroopFactory.h"
#include "TroopStorage.h"
#include "DefenseStation.h"
#include "Outpost.h"
#include "CapitalHub.h"
#include "AlienCity.h"
#include "RockCity.h"
#include "BattleObject.h"
#include "SiegeObject.h"
#include "CapstoneProjectGameModeBase.h"
#include "Types/UnitData.h"
// Alex Code - includes
#include "HexBiomeMaskComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#pragma region General Logic
AGlobalSpawner* AGlobalSpawner::spawnerObject = nullptr;
AGlobalSpawner::AGlobalSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	using SB = SpawnableBuildings;
	using Res = EStratResources;

#pragma region Building Costs
	buildingCosts.Add(SB::MiningStation, FBuildingCost{ 150, 10, 60, EBuildingSize::OneTile, FText::FromString("Mining Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Production_Station.Production_Station'")) });
	buildingCosts.Add(SB::Farmland, FBuildingCost{ 150, 10, 60, EBuildingSize::OneTile, FText::FromString("Farmland"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Food_Farming_Station.Food_Farming_Station'"))});
	buildingCosts.Add(SB::PowerPlant, FBuildingCost{ 150, 10, 60, EBuildingSize::ThreeTiles, FText::FromString("Power Plant"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Energy_Station.Energy_Station'")) });
	buildingCosts.Add(SB::Storage, FBuildingCost{ 150, 10, 60, EBuildingSize::OneTile, FText::FromString("Material Storage"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Material_Storage.Building_Icon_Material_Storage'")) });
	buildingCosts.Add(SB::RobotFactory, FBuildingCost{ 150, 10, 60, EBuildingSize::OneTile, FText::FromString("Robot Factory"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Factory.Building_Icon_Robot_Factory'")) });
	buildingCosts.Add(SB::RobotBarracks, FBuildingCost{ 150, 10, 60, EBuildingSize::OneTile, FText::FromString("Robot Barracks"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Storage.Building_Icon_Robot_Storage'")) });
	buildingCosts.Add(SB::DefenseStation, FBuildingCost{ 150, 10, 60, EBuildingSize::OneTile, FText::FromString("Defense Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Defense_Station.Building_Icon_Defense_Station'")) });
	buildingCosts.Add(SB::Outpost, FBuildingCost{ 0, 10, 60, EBuildingSize::OneTile, FText::FromString("Outpost"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'")) });
	buildingCosts.Add(SB::Capitol, FBuildingCost{0, 10, 0, EBuildingSize::OneTile, FText::FromString("Capitol Hub"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Trade_Outpost_Station.Building_Icon_Trade_Outpost_Station'")) });
#pragma endregion
#pragma region Building Stats
	//Mining Station Stats
	buildingStats.Add(SB::MiningStation, FBuildingStats{ FText::FromString("Mining Station"), FText::FromString("A drilling station that extracts resources necessary for building troops and new facilities."),
		//Resource Yields
		{
			{Res::Production, 2}
		},
		/*Resource Cap Increase*/
		0,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		2,
		/*HP*/
		400,
		/*Vision*/
		1,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		10,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Production_Station.Production_Station'"))
		});
	//Farm Stats
	buildingStats.Add(SB::Farmland, FBuildingStats{ FText::FromString("Farmland"), FText::FromString("Plots of land dedicated to provided the colony with food or cash crops."),
		//Resource Yields
		{
			{Res::Food, 2},
			{Res::Wealth, 2}
		},
		/*Resource Cap Increase*/
		0,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		2,
		/*HP*/
		400,
		/*Vision*/
		1,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		5,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Food_Farming_Station.Food_Farming_Station'"))
		});
	//Power Plant Stats
	buildingStats.Add(SB::PowerPlant, FBuildingStats{ FText::FromString("Power Plant"), FText::FromString("A rudamentory fission reactor that produces energy to power troops and facilities."),
		//Resource Yields
		{
			{Res::Energy, 2}
		},
		/*Resource Cap Increase*/
		0,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		2,
		/*HP*/
		400,
		/*Vision*/
		1,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		0,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Energy_Station.Energy_Station'"))
		});
	//Material Storage Stats
	buildingStats.Add(SB::Storage, FBuildingStats{ FText::FromString("Material Storage"), FText::FromString("A warehouse extension that provides additional storage capacity for the colony's resources."),
		//Resource Yields
		{
			
		},
		/*Resource Cap Increase*/
		500,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		2,
		/*HP*/
		400,
		/*Vision*/
		1,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		0,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Material_Storage.Building_Icon_Material_Storage'"))
		});
	//Robot Factory Stats
	buildingStats.Add(SB::RobotFactory, FBuildingStats{ FText::FromString("Robot Factory"), FText::FromString("A highly industrialized factory capable of creating robotic armies."),
		//Resource Yields
		{
			
		},
		/*Resource Cap Increase*/
		0,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		2,
		/*HP*/
		400,
		/*Vision*/
		1,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		0,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Factory.Building_Icon_Robot_Factory'"))
		});
	//Robot Storage Stats
	buildingStats.Add(SB::RobotBarracks, FBuildingStats{ FText::FromString("Robot Barracks"), FText::FromString("A building that allows for robot units to be repaired when on the tile."),
		//Resource Yields
		{
			
		},
		/*Resource Cap Increase*/
		0,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		2,
		/*HP*/
		400,
		/*Vision*/
		1,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		0,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Storage.Building_Icon_Robot_Storage'"))
		});
	//Defense Station Stats
	buildingStats.Add(SB::DefenseStation, FBuildingStats{ FText::FromString("Defense Station"), FText::FromString("A fortress extension that greatly enhances defensive capabilities."),
		//Resource Yields
		{

		},
		/*Resource Cap Increase*/
		0,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		2,
		/*HP*/
		400,
		/*Vision*/
		1,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		0,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Defense_Station.Building_Icon_Defense_Station'"))
		});
	//Outpost Stats
	buildingStats.Add(SB::Outpost, FBuildingStats{ FText::FromString("Outpost"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited. Create by moving a settler troop to a non-controlled tile, then selecting this."),
		//Resource Yields
		{
			{Res::Energy, 2},
			{Res::Food, 2},
			{Res::Production, 2}
		},
		/*Resource Cap Increase*/
		500,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		6,
		/*HP*/
		400,
		/*Vision*/
		3,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		10,
		/*Max Workers*/
		5,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'"))
		});
	//Capitol Hub
	buildingStats.Add(SB::Capitol, FBuildingStats{ FText::FromString("Capitol Hub"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited."),
		//Resource Yields
		{
			{Res::Energy, 1},
			{Res::Food, 3},
			{Res::Production, 2}
		},
		/*Resource Cap Increase*/
		1000,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		8,
		/*HP*/
		600,
		/*Vision*/
		3,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		20,
		/*Max Workers*/
		15,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Trade_Outpost_Station.Building_Icon_Trade_Outpost_Station'"))
		});
	buildingStats.Add(SB::AlienCity, FBuildingStats{ FText::FromString("Alien City"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited."),
		//Resource Yields
		{
			{Res::Energy, 1},
			{Res::Food, 3},
			{Res::Production, 2}
		},
		/*Resource Cap Increase*/
		1000,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		8,
		/*HP*/
		600,
		/*Vision*/
		3,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		0,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'"))
		});
	buildingStats.Add(SB::RockCity, FBuildingStats{ FText::FromString("Rock City"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited."),
		//Resource Yields
		{
			{Res::Energy, 3},
			{Res::Food, 3},
			{Res::Production, 3}
		},
		/*Resource Cap Increase*/
		1500,
		/*Robot Storage*/
		0,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		10,
		/*HP*/
		1000,
		/*Vision*/
		3,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		5,
		/*Max Workers*/
		15,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'"))
		});
#pragma endregion

#pragma region Troop Costs
	troopCosts.Add(UnitTypes::None, FTroopCost{ 0, 0, 0, FText::FromString("No Troops Building"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Troop_Icon_None.Troop_Icon_None'")) });
	troopCosts.Add(UnitTypes::Infantry, FTroopCost{ 25, 24, 0, FText::FromString("Infantry"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Infantry_Nail.Infantry_Nail'")) });
	troopCosts.Add(UnitTypes::Cavalry, FTroopCost{ 25, 24, 0, FText::FromString("Cavalry"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Cavalry_Nail.Cavalry_Nail'")) });
	troopCosts.Add(UnitTypes::Ranged, FTroopCost{ 25, 24, 0, FText::FromString("Ranged"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Ranged_Nail.Ranged_Nail'")) });
	troopCosts.Add(UnitTypes::Shielder, FTroopCost{ 25, 24, 0, FText::FromString("Shielder"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Shielder_Nail.Shielder_Nail'")) });
	troopCosts.Add(UnitTypes::Scout, FTroopCost{ 25, 12, 0, FText::FromString("Scout"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Scout_Nail.Scout_Nail'")) });
	troopCosts.Add(UnitTypes::Settler, FTroopCost{ 200, 48, 10, FText::FromString("Settler"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Settler_Nail.Settler_Nail'")) });
	troopCosts.Add(UnitTypes::Army, FTroopCost{ 0, 24, 0, FText::FromString("Army"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Troop_Icon_None.Troop_Icon_None'")) });
#pragma endregion
#pragma region Troop Stats
	//Infantry Unit Stats
	troopStats.Add(UnitTypes::Infantry, FTroopStats{ FText::FromString("Infantry"), FText::FromString("Standard robots equiped with melee fuctionality and a short ranged scatterlaser."), UnitTypes::Infantry,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/InfButton.InfButton'")),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/HoveredInfButton.HoveredInfButton'")),
		240, 15, 16, 12, 1, 100, 1, 2,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 150,
		/*Scout*/ 100, 
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Cavalry Unit Stats
	troopStats.Add(UnitTypes::Cavalry, FTroopStats{ FText::FromString("Cavalry"), FText::FromString("Streamlined robots riding hoverbikes that are equiped with titanium tipped lances and laser pistols."), UnitTypes::Cavalry, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/CavalryButton.CavalryButton'")),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/HoveredCavalryButton.HoveredCavalryButton'")),
		180, 11, 20, 8, 1, 100, 1, 2,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 150,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Ranged Unit Stats
	troopStats.Add(UnitTypes::Ranged, FTroopStats{ FText::FromString("Ranged"), FText::FromString("Lighty armored robots that use long ranged laser rifles to devastating effect."), UnitTypes::Ranged,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/RangedButton.RangedButton'")),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/HoveredRangedButton.HoveredRangedButton'")),
		160, 20, 12, 20, 2, 100, 1, 2,
		/*** Attacking ***/
		/*Infantry*/ 150,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Shielder Unit Stats
	troopStats.Add(UnitTypes::Shielder, FTroopStats{ FText::FromString("Shielder"), FText::FromString("Heavily armored robots with thick riot shields and short ranged laser weaponry."), UnitTypes::Shielder,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/ShielderButton.ShielderButton'")),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/HoveredShielderButton.HoveredShielderButton'")),
		280, 19, 8, 16, 1, 100, 1, 2,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 150,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Scout Unit Stats
	troopStats.Add(UnitTypes::Scout, FTroopStats{ FText::FromString("Scout"), FText::FromString("Lightly armored quick robots with enhanced visual capabilities."), UnitTypes::Scout,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/ScoutButton.ScoutButton'")),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/HoveredScoutButton.HoveredScoutButton'")),
		140, 7, 4, 4, 3, 100, 1, 2,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Settler Unit Stats
	troopStats.Add(UnitTypes::Settler, FTroopStats{ FText::FromString("Settler"), FText::FromString("A colony on legs, settlers carry materials and workers to create new outposts. Outposts can be created on any tile a building can be built on OUTSIDE of your zone of control."), UnitTypes::Settler, 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/SettlerButton.SettlerButton'")),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Buttons/HoveredSettlerButton.HoveredSettlerButton'")),
		200, 15, 0, 0, 1, 100, 1, 3,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
#pragma endregion

#pragma region Building Default Prefabs
	buildingPrefabs.Add(SB::MiningStation, AMiningStation::StaticClass());
	buildingPrefabs.Add(SB::Farmland, AFarmland::StaticClass());
	buildingPrefabs.Add(SB::PowerPlant, APowerPlant::StaticClass());
	buildingPrefabs.Add(SB::Storage, AMaterialStorage::StaticClass());
	buildingPrefabs.Add(SB::RobotFactory, ATroopFactory::StaticClass());
	buildingPrefabs.Add(SB::RobotBarracks, ATroopStorage::StaticClass());
	buildingPrefabs.Add(SB::DefenseStation, ADefenseStation::StaticClass());
	buildingPrefabs.Add(SB::Outpost, AOutpost::StaticClass());
	buildingPrefabs.Add(SB::Capitol, ACapitalHub::StaticClass());
	buildingPrefabs.Add(SB::AlienCity, AAlienCity::StaticClass());
	buildingPrefabs.Add(SB::RockCity, ARockCity::StaticClass());
#pragma endregion
}
void AGlobalSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (!troopPrefab) troopPrefab = ATroop::StaticClass();
	if (!mergedArmyPrefab) mergedArmyPrefab = AMergedArmy::StaticClass();
	if (!battlePrefab) battlePrefab = ABattleObject::StaticClass();
	if (!siegePrefab) siegePrefab = ASiegeObject::StaticClass();

	spawnerObject = this;
	AActor* controllerTemp = UGameplayStatics::GetActorOfClass(GetWorld(), ABasePlayerController::StaticClass());
	controller = Cast<ABasePlayerController>(controllerTemp);
}
#pragma endregion

#pragma region Building Construction
bool AGlobalSpawner::SpawnBuilding(EFactions faction, SpawnableBuildings building, ABaseHex* hex)
{
	if (building == SpawnableBuildings::Outpost) return SpawnOutpost(faction, TArray<ABaseHex*>({hex}));

	if (building != SpawnableBuildings::Capitol && building != SpawnableBuildings::AlienCity && building != SpawnableBuildings::RockCity)
	{
		//Check if terrain is valid
		if (!hex->CanBuildOnHex(buildingCosts[building].size))
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Cannot construct building, not enough space."));
			return false;
		}
	}

	//Determine building prefab to spawn
	UClass* prefab = DetermineBuildingType(building);

	//If no prefab found, return
	if (!prefab) return false;

	UFaction* factionObject = UnitActions::GetFaction(faction);

	if (CanAffordBuilding(factionObject, building))
	{
		ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(prefab);
		factionObject->SetResources({ { EStratResources::Production, -buildingCosts[building].productionCost } });
		newBuilding->InitBuilding(faction);
		newBuilding->AttachToHex(hex);

		return true;
	}

	return false;
}
bool AGlobalSpawner::SpawnOutpost(EFactions faction, const TArray<ABaseHex*>& hexes)
{
	UFaction* factionObject = UnitActions::GetFaction(faction);
	UClass* prefab = DetermineBuildingType(SpawnableBuildings::Outpost);
	if (!prefab) return false;

	if (!CanAffordBuilding(factionObject, SpawnableBuildings::Outpost)) return false;

	AMovementAI* settlerOnHex = nullptr;
	int settlerIndex = -1;

	for (ABaseHex* hex : hexes)
	{
		if (!hex->IsBuildableTerrain() || hex->building || hex->GetHexOwner() != EFactions::None) return false;

		if (!settlerOnHex)
		{
			for (int i = 0; i < hex->troopsInHex.Num(); i++)
			{
				if (hex->troopsInHex[i]->GetUnitData()->GetFaction() != faction) continue;

				if (UnitActions::ArmyContainsUnit(hex->troopsInHex[i], UnitTypes::Settler, settlerIndex))
				{
					settlerOnHex = hex->troopsInHex[i];
					break;
				}
			}
		}
	}

	if (!settlerOnHex) return false;
	if (settlerOnHex->GetUnitData()->GetUnitType() == UnitTypes::Settler) settlerOnHex->Destroy();
	else settlerOnHex->GetUnitData()->ExtractUnitData(settlerIndex, true);

	ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(prefab);
	factionObject->SetResources({ { EStratResources::Production, -buildingCosts[SpawnableBuildings::Outpost].productionCost } });
	newBuilding->InitBuilding(faction);
	newBuilding->AttachToHex(hexes);

	return true;
}
void AGlobalSpawner::SpawnBuildingFree(EFactions faction, SpawnableBuildings building, ABaseHex* hex, bool buildAtStart)
{
	ABuilding* spawn = GetWorld()->SpawnActor<ABuilding>(DetermineBuildingType(building));
	spawn->InitBuilding(faction);
	spawn->AttachToHex(hex);
	spawn->SetBuildAtStart(buildAtStart);
}
UClass* AGlobalSpawner::DetermineBuildingType(SpawnableBuildings building)
{
	if (!buildingPrefabs.Contains(building))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Failed to build: building prefab not found"));
		return nullptr;
	}

	return buildingPrefabs[building];
}
bool AGlobalSpawner::CanAffordBuilding(UFaction* faction, SpawnableBuildings building) const
{
	if (!buildingCosts.Contains(building)) return false;

	return faction->CanAffordResource(EStratResources::Production, buildingCosts[building].productionCost);
}
EStratResources AGlobalSpawner::GetMainBuildingYield(SpawnableBuildings building)
{
	if (spawnerObject == nullptr) return EStratResources::None;
	if (!spawnerObject->buildingStats.Contains(building)) return EStratResources::None;

	const TMap<EStratResources, int32>& yields = spawnerObject->buildingStats[building].resourceYields;

	EStratResources best = EStratResources::None;
	for (const TPair<EStratResources, int32>& resource : yields)
	{
		if (resource.Key == EStratResources::Wealth) continue;

		if (best == EStratResources::None || yields[resource.Key] > yields[best])
		{
			best = resource.Key;
		}
	}

	return best;
}
bool AGlobalSpawner::BuildingOnHex(ABaseHex* hex)
{
	if (!hex) return false;

	FHitResult hitResult;
	FCollisionQueryParams params;
	params.AddIgnoredActor(hex);

	FVector start = hex->GetActorLocation();
	FVector end = start + FVector::UpVector * 50.f;

	bool hit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, params);

	if (hit)
	{
		ABuilding* building = Cast<ABuilding>(hitResult.GetActor());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Building on hex, skipping"));
		if (building) return true;
	}

	return false;
}
#pragma endregion

#pragma region Troop/Battle Construction
ATroop* AGlobalSpawner::SpawnArmyByUnit(ABaseHex* hex, FUnitData* data, float parentHealthPercent)
{
	FActorSpawnParameters params;
	if (!troopPrefab || !mergedArmyPrefab) return nullptr;

	UClass* prefab = data->GetUnitType() == UnitTypes::Army ? mergedArmyPrefab : troopPrefab;

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(prefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);

	newTroop->InitTroop(data);
	data->SetHPByAlpha(parentHealthPercent, false);

	return newTroop;
}
AMergedArmy* AGlobalSpawner::SpawnArmyByArray(ABaseHex* hex, TArray<FUnitData*> groupData, float parentHealthPercent)
{
	FActorSpawnParameters params;
	if (!mergedArmyPrefab) return nullptr;

	AMergedArmy* newTroop = GetWorld()->SpawnActor<AMergedArmy>(mergedArmyPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);

	for (int i = 0; i < groupData.Num(); ++i)
	{
		groupData[i]->SetHPByAlpha(parentHealthPercent, false);
	}

	newTroop->InitTroop(groupData[0]->GetFaction(), UnitTypes::Army);
	newTroop->ConsumeData(groupData);

	return newTroop;
}
void AGlobalSpawner::MergeArmies(ATroop* seeker, ATroop* target, ABaseHex* hex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("Armies merged!"));
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
		mergedArmy = Cast<AMergedArmy>(BuildArmy(seeker->GetUnitData()->GetFaction(), hex));
		mergedArmy->ConsumeUnit(seeker);
		mergedArmy->ConsumeUnit(target);
		break;

		//If one unit is an army, determine which unit is that army and merge the other into it
	case 1:
		AMergedArmy* army;
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

		army->ConsumeUnit(unit);
		break;

		//If both units are armies, let the seeker be the dominant army and merge the other army into it
	case 2:
		testSeeker->ConsumeUnit(testTarget);
		break;
	}
}

bool AGlobalSpawner::PurchaseTroop(EFactions faction, UnitTypes unit)
{
	bool canAfford = false;

	TMap<EStratResources, int> resourceCosts = TMap<EStratResources, int>();
	if (troopCosts.Contains(unit))
	{
		TMap<EStratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);

		if (resources[EStratResources::Production] >= troopCosts[unit].productionCost) 
			canAfford = true;

		resourceCosts.Add(EStratResources::Production, -troopCosts[unit].productionCost);
		if (unit == UnitTypes::Settler)
			resourceCosts.Add(EStratResources::Population, -troopCosts[unit].populationCost);
	}

	if (canAfford)
	{
		UnitActions::GetFaction(faction)->SetResources(resourceCosts);
		controller->PlayUITroopSound(unit);
	}
	else
	{
		controller->PlayUISound(controller->selectFailSound);
	}

	return canAfford;
}
ATroop* AGlobalSpawner::BuildTroop(EFactions faction, UnitTypes unit, ABaseHex* hex)
{
	if (unit == UnitTypes::Army) return BuildArmy(faction, hex);

	FTroopStats unitData = troopStats[unit];

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	newTroop->InitTroop(faction, unit);

	return newTroop;
}
ATroop* AGlobalSpawner::BuildArmy(EFactions faction, ABaseHex* hex)
{
	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(mergedArmyPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	newTroop->InitTroop(faction, UnitTypes::Army);

	return newTroop;
}

ABattleObject* AGlobalSpawner::SpawnBattle(ABaseHex* hex)
{
	if (hex->building)
		hex = hex->building->hexNav->GetCurrentHex();

	FActorSpawnParameters params;
	ABattleObject* battle = GetWorld()->SpawnActor<ABattleObject>(hex->building ? siegePrefab : battlePrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0.f, 0.f, 0.f), params);
	battle->hexNav->SetCurrentHex(hex);
	battle->Start();

	return battle;
}
#pragma endregion

#pragma region Particle Effects
AActor* AGlobalSpawner::SpawnSmoke(AActor* object)
{
	if (!smokePrefab) return nullptr;

	FActorSpawnParameters params;
	AActor* particleSystem = GetWorld()->SpawnActor<AActor>(smokePrefab, object->GetActorLocation(), FRotator(0.f, 0.f, 0.f), params);

	return particleSystem;
}
AActor* AGlobalSpawner::SpawnEndParticle(AActor* object, GameStates state)
{
	AActor* particleSystem = nullptr;
	FActorSpawnParameters params;

	switch (state)
	{
	case GameStates::None:
		break;
	case GameStates::Defeat:
		if (!explosionPrefab) return nullptr;
		particleSystem = GetWorld()->SpawnActor<AActor>(explosionPrefab, object->GetActorLocation(), FRotator(0.f, 0.f, 0.f), params);
		break;
	case GameStates::Victory:
		if (!fireWorksPrefab)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, TEXT("No prefab"));
			return nullptr;
		}
		particleSystem = GetWorld()->SpawnActor<AActor>(fireWorksPrefab, object->GetActorLocation(), FRotator(0.f, 0.f, 0.f), params);
		break;
	}

	return particleSystem;
}
#pragma endregion