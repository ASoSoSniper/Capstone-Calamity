// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalSpawner.h"
#include "BasePlayerController.h"
#include "BaseHex.h"
#include "HexNav.h"
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

#pragma region General Logic
AGlobalSpawner* AGlobalSpawner::spawnerObject = nullptr;
AGlobalSpawner::AGlobalSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	using SB = SpawnableBuildings;

#pragma region Building Costs
	buildingCosts.Add(SB::MiningStation, FBuildingCost{ 150, 10, 60, 0, FText::FromString("Mining Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Production_Station.Production_Station'")) });
	buildingCosts.Add(SB::Farmland, FBuildingCost{ 150, 10, 60, 0, FText::FromString("Farmland"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Food_Farming_Station.Food_Farming_Station'"))});
	buildingCosts.Add(SB::PowerPlant, FBuildingCost{ 150, 10, 60, 1, FText::FromString("Power Plant"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Energy_Station.Energy_Station'")) });
	buildingCosts.Add(SB::Storage, FBuildingCost{ 150, 10, 60, 0, FText::FromString("Material Storage"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Material_Storage.Building_Icon_Material_Storage'")) });
	buildingCosts.Add(SB::RobotFactory, FBuildingCost{ 150, 10, 60, 0, FText::FromString("Robot Factory"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Factory.Building_Icon_Robot_Factory'")) });
	buildingCosts.Add(SB::RobotBarracks, FBuildingCost{ 150, 10, 60, 0, FText::FromString("Robot Barracks"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Storage.Building_Icon_Robot_Storage'")) });
	buildingCosts.Add(SB::DefenseStation, FBuildingCost{ 150, 10, 60, 0, FText::FromString("Defense Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Defense_Station.Building_Icon_Defense_Station'")) });
	buildingCosts.Add(SB::Outpost, FBuildingCost{ 0, 10, 60, 0, FText::FromString("Outpost"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'")) });
	buildingCosts.Add(SB::Capitol, FBuildingCost{0, 10, 0, 0, FText::FromString("Capitol Hub"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Trade_Outpost_Station.Building_Icon_Trade_Outpost_Station'")) });
#pragma endregion
#pragma region Building Stats
	//Mining Station Stats
	buildingStats.Add(SB::MiningStation, FBuildingStats{ FText::FromString("Mining Station"), FText::FromString("A drilling station that extracts resources necessary for building troops and new facilities."),
		/*Energy Yield*/
		0,
		/*Food Yield*/
		0,
		/*Production Yield*/
		2,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		0,
		/*Food Yield*/
		2,
		/*Production Yield*/
		0,
		/*Wealth Yield*/
		2,
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
		/*Energy Yield*/
		2,
		/*Food Yield*/
		0,
		/*Production Yield*/
		0,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		0,
		/*Food Yield*/
		0,
		/*Production Yield*/
		0,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		0,
		/*Food Yield*/
		0,
		/*Production Yield*/
		0,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		0,
		/*Food Yield*/
		0,
		/*Production Yield*/
		0,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		0,
		/*Food Yield*/
		0,
		/*Production Yield*/
		0,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		2,
		/*Food Yield*/
		2,
		/*Production Yield*/
		2,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		1,
		/*Food Yield*/
		3,
		/*Production Yield*/
		2,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		1,
		/*Food Yield*/
		3,
		/*Production Yield*/
		2,
		/*Wealth Yield*/
		0,
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
		/*Energy Yield*/
		3,
		/*Food Yield*/
		3,
		/*Production Yield*/
		3,
		/*Wealth Yield*/
		0,
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

	ProceduralHexGen(400, ShapesOfMap::Square);
}
void AGlobalSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!controller)
	{
		AActor* controllerTemp = UGameplayStatics::GetActorOfClass(GetWorld(), ABasePlayerController::StaticClass());
		controller = Cast<ABasePlayerController>(controllerTemp);
	}
}
FFactionDisplay* AGlobalSpawner::GetFactionDisplayPreset(Factions faction)
{
	if (!factionDisplayPresets.Contains(faction)) return nullptr;

	return &factionDisplayPresets[faction];
}
#pragma endregion

#pragma region Hex Generation
void AGlobalSpawner::CreateHexModel(TerrainType terrainType, ABaseHex* hex)
{
	UStaticMesh* meshAsset = nullptr;
	UStaticMesh* extraAsset = nullptr;

	ABuilding* newBuilding = nullptr;

	switch (terrainType)
	{
	case TerrainType::None:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TilePlains.TilePlains'"));
		break;
	case TerrainType::Plains:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TilePlains.TilePlains'"));
		break;
	case TerrainType::Hills:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileHills.TileHills'"));
		break;
	case TerrainType::Mountains:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileMountainModel.TileMountainModel'"));
		break;
	case TerrainType::Forest:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForest.TileForest'"));
		extraAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForestModel_TileForest.TileForestModel_TileForest'"));
		break;
	case TerrainType::Jungle:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'"));
		extraAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_TileForestThick.TileJungleModel_TileForestThick'"));
		break;
	case TerrainType::SporeField:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileSporefieldModel.TileSporefieldModel'"));
		break;
	case TerrainType::Border:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'"));
		break;
	case TerrainType::Ship:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForest.TileForest'"));
		extraAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingCapitolShip2.BuildingCapitolShip2'"));
		break;
	case TerrainType::AlienCity:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'"));

		newBuilding = GetWorld()->SpawnActor<ABuilding>(DetermineBuildingType(SpawnableBuildings::AlienCity), hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));

		break;
	case TerrainType::TheRock:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'"));

		newBuilding = GetWorld()->SpawnActor<ABuilding>(DetermineBuildingType(SpawnableBuildings::RockCity), hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));

		break;
	default:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/CyTheHexGuy.CyTheHexGuy'"));
		break;
	}

	if (meshAsset)
	{
		hex->hexMesh->SetStaticMesh(meshAsset);
		hex->hexMesh->EmptyOverrideMaterials();
	}

	if (extraAsset)
	{
		hex->hexMeshAttachment->SetStaticMesh(extraAsset);
		hex->hexMeshAttachment->EmptyOverrideMaterials();
		float randomRot = FMath::RandRange(0.f, 360.f);

		switch (terrainType)
		{

		case TerrainType::Jungle:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, randomRot, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		case TerrainType::Mountains:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, randomRot, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		case TerrainType::Forest:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, randomRot, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		case TerrainType::SporeField:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, randomRot, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		case TerrainType::Hills:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, randomRot, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		case TerrainType::Plains:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, randomRot, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		case TerrainType::Ship:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, -45.f, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		default:
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, 0, 0));
			hex->hexMeshAttachment->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
			break;
		}
	}

	hex->hexMeshAttachment->SetVisibility(extraAsset != nullptr);
	hex->SetAttachmentCanBeVisible(extraAsset != nullptr);

	Factions owner = hex->GetHexOwner();

	hex->visibility->ResetComponent();
	hex->visibility->SetupComponent(owner, hex->hexMesh);
	hex->visibility->SetupComponent(owner, hex->hexMeshAttachment);
	hex->visibility->SetupFactionComponent(hex->hexBase);
}
ABaseHex* AGlobalSpawner::GetHexFromCoordinates(int x, int y)
{
	if (x < 0 || y < 0) return nullptr;

	if (hexArray.Num() - 1 > x)
	{
		if (hexArray[x].Num() - 1 > y)
		{
			return hexArray[x][y];
		}
	}

	return nullptr;
}
void AGlobalSpawner::ProceduralHexGen(int numHexs, ShapesOfMap shape)
{
	bool shipExists = false;
	bool rockExists = false;
	float stepDistanceX = 73.5f;
	float stepDistanceY = -63.f;
	FVector origin = FVector(-250.f, 400.f, -1.f);
	float sqroot;
	int roundedSQRoot;
	ABaseHex* newHex;
	ABaseHex* rockHex;

	TArray<TArray<ABaseHex*>> arrayOfHexColumns;
	TArray<FVector2D> usedCoordinates;

	switch (shape)
	{
	case ShapesOfMap::None:
		break;
	case ShapesOfMap::Square:
		sqroot = FMath::Sqrt((float)numHexs);
		roundedSQRoot = sqroot;
		if (roundedSQRoot % 2 > 0)
		{
			roundedSQRoot += 1;
		}

		for (int x = 0; x < roundedSQRoot; x++)
		{
			TArray<ABaseHex*> column;

			for (int y = 0; y < roundedSQRoot; y++)
			{
				float oddHex = (y % 2 > 0) ? 1 : 0;

				FVector spawnPos = origin + FVector(-(stepDistanceY * y), -(stepDistanceX * x + (oddHex * stepDistanceX / 2)), 0.f);

				newHex = GetWorld()->SpawnActor<ABaseHex>(hexActor, spawnPos, FRotator::ZeroRotator);
				newHex->SetHexCoordinates(x, y);
				newHex->PrintCoordinates(x, y);

				column.Add(newHex);
			}

			arrayOfHexColumns.Add(column);
		}

		hexArray = arrayOfHexColumns;

		for (int x = 0; x < hexArray.Num(); x++)
		{
			for (int y = 0; y < hexArray[x].Num(); y++)
			{
				hexArray[x][y]->SetHexTerrain(hexSeedSize, hexRandToMaintain);
			}
		}

		hexArray[4][3]->SetHexTerrain(TerrainType::Ship);

		rockHex = hexArray[FMath::RandRange(12, 17)][FMath::RandRange(12, 17)];
		rockHex->SetHexTerrain(TerrainType::TheRock);
		SpawnBuildingsAroundCity(rockHex);

		for (int i = 0; i < 6; i++)
		{
			ABaseHex* hex = nullptr;
			FVector2D randomXY = FVector2D::Zero();

			while (!hex)
			{
				randomXY = FVector2D(FMath::RandRange(1, 19), FMath::RandRange(1, 19));
				if (randomXY.X < 10 && randomXY.Y < 10) continue;

				if (!usedCoordinates.Contains(randomXY))
				{
					ABaseHex* hexTest = hexArray[randomXY.X][randomXY.Y];
					if (!hexTest->IsStaticBuildingTerrain() && !BuildingOnHex(hexTest))
					{
						usedCoordinates.Add(randomXY);
						hex = hexTest;
					}
				}
			}

			hex->SetHexTerrain(TerrainType::AlienCity);
			SpawnBuildingsAroundCity(hex);
		}

		SpawnPointsOfInterest();

		break;
	case ShapesOfMap::Rectangle:
		break;
	case ShapesOfMap::Catan:
		break;
	case ShapesOfMap::Circle:
		break;
	default:
		break;
	}
}
void AGlobalSpawner::SpawnBuildingsAroundCity(ABaseHex* centerHex)
{
	FVector2D center = centerHex->GetHexCoordinates();
	if (center == FVector2D::Zero())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Could not get coordinates"));
		return;
	}
	TArray<ABaseHex*> randomHexes;
	TArray<FVector2D> claimedCoordinates;
	for (int i = 0; i < 3; i++)
	{
		ABaseHex* hex = nullptr;
		int randX = 0;
		int randY = 0;

		while (!hex)
		{
			randX = FMath::RandRange(center.X - buildingDistanceFromCity, center.X + buildingDistanceFromCity);
			randY = FMath::RandRange(center.Y - buildingDistanceFromCity, center.Y + buildingDistanceFromCity);

			if (!claimedCoordinates.Contains(FVector2D(randX, randY)))
			{
				ABaseHex* hexTest = GetHexFromCoordinates(randX, randY);
				if (!hexTest) continue;

				if (!hexTest->IsStaticBuildingTerrain() &&
					!BuildingOnHex(hexTest))
				{
					hex = hexTest;
				}

			}
		}

		randomHexes.Add(hex);
		claimedCoordinates.Add(FVector2D(randX, randY));
	}

	SpawnBuildingFree(Factions::Alien1, SpawnableBuildings::MiningStation, randomHexes[0], true);
	SpawnBuildingFree(Factions::Alien1, SpawnableBuildings::Farmland, randomHexes[1], true);
	SpawnBuildingFree(Factions::Alien1, SpawnableBuildings::PowerPlant, randomHexes[2], true);
}
void AGlobalSpawner::SpawnPointsOfInterest()
{
	if (pointsOfInterest.IsEmpty()) return;

	int32 xMax = hexArray.Num() - 1;
	int32 yMax = hexArray[0].Num() - 1;

	for (int i = 0; i < pointOfInterestCount; i++)
	{
		int32 randPOI = FMath::RandRange(0, pointsOfInterest.Num() - 1);

		while (true)
		{
			int32 x = FMath::RandRange(0, xMax);
			int32 y = FMath::RandRange(0, yMax);

			if (hexArray[x][y]->HasPOI()) continue;

			hexArray[x][y]->CreatePointOfInterest(pointsOfInterest[randPOI]);
			break;
		}
	}
}
#pragma endregion

#pragma region Building Construction
void AGlobalSpawner::SpawnBuilding(Factions faction, SpawnableBuildings building, ABaseHex* hex)
{
	//If hex already occupied
	if (hex->building)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Hex already occupied"));
		return;
	}

	//Check if terrain is valid
	TSet<ABaseHex*> hexesToBuild = hex->GetHexesInRadius(buildingCosts[building].hexLayers);
	for (ABaseHex* aHex : hexesToBuild)
	{
		if (aHex->building ||
			!aHex->IsBuildableTerrain() ||
			aHex->GetHexOwner() != faction)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Not valid terrain, cannot build"));
			return;
		}
	}

	//Determine building prefab to spawn
	FActorSpawnParameters params;
	UClass* prefab = DetermineBuildingType(building);

	//If no prefab found, return
	if (!prefab) return;

	//Declare bool for whether this building is affordable
	bool canAfford = false;

	//Declare TMaps for resources and workers to spend on this building
	TMap<EStratResources, int> resourceCosts = TMap<EStratResources, int>();
	AMovementAI* settlerOnHex = nullptr;
	int settlerIndex = -1;

	//If the desired building exists in the 
	if (buildingCosts.Contains(building))
	{
		TMap<EStratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);

		if (resources[EStratResources::Production] >= buildingCosts[building].productionCost)
		{
			if (building == SpawnableBuildings::Outpost)
			{
				for (int i = 0; i < hex->troopsInHex.Num(); i++)
				{
					if (UnitActions::ArmyContainsUnit(hex->troopsInHex[i], UnitTypes::Settler, settlerIndex))
					{
						if (hex->troopsInHex[i]->GetUnitData()->GetFaction() == faction)
						{
							canAfford = true;
							settlerOnHex = hex->troopsInHex[i];
						}
					}
				}
			}
			else
			{
				canAfford = true;
			}
		}

		resourceCosts.Add(EStratResources::Production, buildingCosts[building].productionCost);
	}

	if (canAfford)
	{
		ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(prefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
		UnitActions::ConsumeSpentResources(faction, resourceCosts, hex);
		newBuilding->InitBuilding(faction);

		if (building == SpawnableBuildings::Outpost)
		{
			UnitActions::SetWorkers(faction, WorkerType::Human, troopCosts[UnitTypes::Settler].populationCost);
			if (settlerOnHex)
			{
				if (settlerOnHex->GetUnitData()->GetUnitType() == UnitTypes::Settler)
				{
					settlerOnHex->Destroy();
				}
				else
				{
					settlerOnHex->GetUnitData()->ExtractUnitData(settlerIndex, true);
				}
			}
		}

		controller->PlayUIBuildingSound(building);
	}
	else
	{
		controller->PlayUISound(controller->selectFailSound);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Cannot afford building"));
	}
}
void AGlobalSpawner::SpawnBuildingFree(Factions faction, SpawnableBuildings building, ABaseHex* hex, bool buildAtStart)
{
	ABuilding* spawn = GetWorld()->SpawnActor<ABuilding>(DetermineBuildingType(building), hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	spawn->InitBuilding(faction);
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

bool AGlobalSpawner::PurchaseTroop(Factions faction, UnitTypes unit)
{
	bool canAfford = false;

	TMap<EStratResources, int> resourceCosts = TMap<EStratResources, int>();
	if (troopCosts.Contains(unit))
	{
		TMap<EStratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);

		if (resources[EStratResources::Production] >= troopCosts[unit].productionCost)
		{
			if (unit == UnitTypes::Settler)
			{
				if (UnitActions::SetWorkers(faction, WorkerType::Human, -troopCosts[UnitTypes::Settler].populationCost))
				{
					canAfford = true;
				}
			}
			else
			{
				canAfford = true;
			}

		}

		resourceCosts.Add(EStratResources::Production, troopCosts[unit].productionCost);
	}

	if (canAfford)
	{
		UnitActions::ConsumeSpentResources(faction, resourceCosts, nullptr);
		controller->PlayUITroopSound(unit);
	}
	else
	{
		controller->PlayUISound(controller->selectFailSound);
	}

	return canAfford;
}
ATroop* AGlobalSpawner::BuildTroop(Factions faction, UnitTypes unit, ABaseHex* hex)
{
	if (unit == UnitTypes::Army) return BuildArmy(faction, hex);

	FTroopStats unitData = troopStats[unit];

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	newTroop->InitTroop(faction, unit);

	return newTroop;
}
ATroop* AGlobalSpawner::BuildArmy(Factions faction, ABaseHex* hex)
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

#pragma region UnitData
Factions FUnitData::GetFaction() const
{
	return faction;
}
UnitTypes FUnitData::GetUnitType() const
{
	return unitType;
}

void FUnitData::SetUnitValues(int setHealth, int setMorale, int setVision, int setSpeed, int setDamage, int setSiegePower, int setReinforceRate, int setEnergyUpKeep)
{
	currentHP = setHealth;
	maxHP = setHealth;
	currentMorale = setMorale;
	maxMorale = setMorale;

	vision = setVision;
	speed = setSpeed;

	damage = setDamage;
	siegePower = setSiegePower;

	reinforceRate = setReinforceRate;
	energyUpkeep = setEnergyUpKeep;
}
void FUnitData::AddUnitData(FUnitData* data)
{
	if (unitType != UnitTypes::Army) return;

	if (data->GetUnitType() == UnitTypes::Army)
	{
		TArray<FUnitData*> units = data->GetSavedUnits();
		for (int i = 0; i < units.Num(); i++)
		{
			AddUnitData(units[i]);
		}

		return;
	}

	for (FStatusEffect* effect : data->GetStatusEffects())
	{
		AddStatusEffect(effect);
	}

	savedUnits.Add(data);
	data->DestroyWorldData();

	currentHP += data->currentHP;
	maxHP += data->maxHP;
	currentMorale += data->currentMorale;
	maxMorale += data->maxMorale;

	int setSpeed = 0;
	int bestVision = 0;
	for (int i = 0; i < savedUnits.Num(); i++)
	{
		setSpeed += savedUnits[i]->speed;

		if (savedUnits[i]->vision > bestVision)
			bestVision = savedUnits[i]->vision;
	}
	setSpeed = FMath::RoundToInt((float)setSpeed / (float)savedUnits.Num());

	speed = setSpeed;
	vision = bestVision;

	damage += data->damage;
	siegePower += data->siegePower;

	reinforceRate += data->reinforceRate;
	energyUpkeep += data->energyUpkeep;
}
FUnitData* FUnitData::ExtractUnitData(int32 index, bool killOnExtraction)
{
	if (!savedUnits.IsValidIndex(index)) return nullptr;

	FUnitData* unit = savedUnits[index];
	savedUnits.RemoveAt(index);

	float hpPercent = (float)currentHP / (float)maxHP;
	float moralePercent = (float)currentMorale / (float)maxMorale;

	maxHP -= unit->maxHP;
	currentHP = FMath::RoundToInt((float)maxHP * hpPercent);
	maxMorale -= unit->maxMorale;
	currentMorale = FMath::RoundToInt((float)maxMorale * moralePercent);

	int setSpeed = 0;
	int bestVision = 0;
	for (int i = 0; i < savedUnits.Num(); i++)
	{
		setSpeed += savedUnits[i]->speed;

		if (savedUnits[i]->vision > bestVision)
			bestVision = savedUnits[i]->vision;
	}
	setSpeed = FMath::RoundToInt((float)setSpeed / (float)savedUnits.Num());

	speed = setSpeed;
	vision = bestVision;

	damage -= unit->damage;
	siegePower -= unit->siegePower;

	reinforceRate -= unit->reinforceRate;
	energyUpkeep -= unit->energyUpkeep;

	if (killOnExtraction)
	{
		return nullptr;
	}
	else
	{
		unit->currentHP = FMath::RoundToInt((float)unit->maxHP * hpPercent);
		unit->currentMorale = FMath::RoundToInt((float)unit->maxMorale * hpPercent);
		GenerateArmyName();
	}

	return unit;
}

void FUnitData::SetBuildingValues(int setHealth, int setVision, int setSiegePower, int setEnergyUpkeep)
{
	currentHP = setHealth;
	maxHP = setHealth;

	vision = setVision;

	damage = setSiegePower;
	siegePower = setSiegePower;
	energyUpkeep = setEnergyUpkeep;

	isBuilding = true;
}

void FUnitData::GenerateArmyName(FString newName)
{
	using GameMode = ACapstoneProjectGameModeBase;

	if (!GameMode::activeFactions.Contains(faction)) return;
	if (armyName == newName && newName != TEXT("")) return;

	TMap<FString, TArray<int32>>& names = faction == Factions::Human ?
		GameMode::activeFactions[faction]->armyNamesHuman :
		GameMode::activeFactions[faction]->armyNamesAlien;

	int32& cap = GameMode::activeFactions[faction]->maxNameShare;

	RemoveArmyName();
	if (!names.Contains(newName)) names.Add(newName, TArray<int32>());

	if (newName != TEXT(""))
	{
		if (names[newName].Num() < cap)
		{
			int32 num = 0;
			for (int i = 1; i < cap; i++)
			{
				if (!names[newName].Contains(i))
				{
					num = i;
					names[newName].Add(i);
					break;
				}
			}

			armyName = newName;
			nameInstance = num;
			if (num != 0) return;
		}
	}

	for (auto& name : names)
	{
		if (name.Value.Num() < cap)
		{
			int32 num = 0;
			for (int i = 1; i < cap; i++)
			{
				if (!name.Value.Contains(i))
				{
					num = i;
					name.Value.Add(i);
					break;
				}
			}

			armyName = name.Key;
			nameInstance = num;

			if (num != 0) break;
		}
	}
}
void FUnitData::RemoveArmyName()
{
	if (!ACapstoneProjectGameModeBase::activeFactions.Contains(faction)) return;

	TMap<FString, TArray<int32>>& names = faction == Factions::Human ?
		ACapstoneProjectGameModeBase::activeFactions[faction]->armyNamesHuman :
		ACapstoneProjectGameModeBase::activeFactions[faction]->armyNamesAlien;

	if (names.Contains(armyName) && names[armyName].Contains(nameInstance))
	{
		names[armyName].Remove(nameInstance);
	}
}

FText FUnitData::GetArmyName() const
{
	if (unitType != UnitTypes::Army)
		return AGlobalSpawner::spawnerObject->troopStats[unitType].title;

	return FText::FromString(armyName + TEXT(" No. ") + FString::FromInt(nameInstance));
}
FString FUnitData::GetNameRaw() const
{
	return armyName;
}
int FUnitData::GetNameInstance() const
{
	return nameInstance;
}

int FUnitData::GetCurrentHP() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetHPMod();
	}

	return FMath::RoundToInt(static_cast<float>(currentHP) * totalMulti);
}
int FUnitData::GetMaxHP() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetHPMod();
	}

	return FMath::RoundToInt(static_cast<float>(maxHP) * totalMulti);
}
void FUnitData::SetHPByAlpha(float alpha, bool allowDeath)
{
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	currentHP = FMath::RoundToInt((float)maxHP * alpha);

	currentHP = FMath::Max(currentHP, allowDeath ? 0 : 1);
}
int FUnitData::GetCurrentMorale() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetMoraleMod();
	}

	return FMath::RoundToInt(static_cast<float>(currentMorale) * totalMulti);
}
int FUnitData::GetMaxMorale() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetMoraleMod();
	}

	return FMath::RoundToInt(static_cast<float>(maxMorale) * totalMulti);
}
void FUnitData::SetMoraleByAlpha(float alpha)
{
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	currentMorale = FMath::RoundToInt(static_cast<float>(maxMorale) * alpha);
}
void FUnitData::SetMoraleByValue(int value)
{
	currentMorale = FMath::Clamp(value, 0, maxMorale);
}
float FUnitData::GetHPAlpha() const
{
	float curr = static_cast<float>(GetCurrentHP());
	float max = static_cast<float>(GetMaxHP());

	return FMath::Clamp(curr / max, 0.f, 1.f);
}
float FUnitData::GetMoraleAlpha() const
{
	float curr = static_cast<float>(GetCurrentMorale());
	float max = static_cast<float>(GetMaxMorale());

	return FMath::Clamp(curr / max, 0.f, 1.f);
}
bool FUnitData::IsAlive() const
{
	return currentHP > 0;
}
bool FUnitData::HasMorale() const
{
	return currentMorale > 0;
}

int FUnitData::DamageHP(int amount)
{
	int hp = GetCurrentHP();
	int max = GetMaxHP();

	hp -= amount;
	float alpha = static_cast<float>(hp) / static_cast<float>(max);

	SetHPByAlpha(alpha);

	return hp;
}
int FUnitData::DamageMorale(int amount)
{
	int morale = GetCurrentMorale();
	int max = GetMaxMorale();

	morale -= amount;
	float alpha = static_cast<float>(morale) / static_cast<float>(max);

	SetMoraleByAlpha(alpha);

	return morale;
}

int FUnitData::HealHP(int amount)
{
	currentHP += amount;
	currentHP = FMath::Clamp(currentHP, 0, maxHP);

	return currentHP;
}

int FUnitData::GetVision() const
{
	int totalVisionMod = 0;

	for (FStatusEffect* effect : statusEffects)
	{
		totalVisionMod += effect->GetVisionMod();
	}

	return vision + totalVisionMod;
}
int FUnitData::GetSpeed() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetSpeedMod();
	}

	return FMath::RoundToInt(static_cast<float>(speed) * totalMulti);
}
int FUnitData::GetDamage() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetDamageMod();
	}

	return FMath::RoundToInt(static_cast<float>(damage) * totalMulti);
}
int FUnitData::GetSiegePower() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetSiegePowerMod();
	}

	return FMath::RoundToInt(static_cast<float>(siegePower) * totalMulti);
}
int FUnitData::GetReinforceRate() const
{
	return reinforceRate;
}
int FUnitData::GetEnergyUpkeep() const
{
	return energyUpkeep;
}
bool FUnitData::IsBuilding() const 
{
	return isBuilding;
}
const TArray<FUnitData*>& FUnitData::GetSavedUnits() const
{
	return savedUnits;
}
int FUnitData::GetSavedUnitCount() const
{
	return savedUnits.Num();
}

bool FUnitData::SetupComplete() const
{
	return maxHP > 0;
}
void FUnitData::DestroyWorldData()
{
	RemoveArmyName();
	ClearStatusEffects();
}

const TSet<FStatusEffect*>& FUnitData::GetStatusEffects() const
{
	return statusEffects;
}
void FUnitData::AddStatusEffect(FStatusEffect* effect)
{
	statusEffects.Add(effect);
}
void FUnitData::RemoveStatusEffect(FStatusEffect* effect)
{
	if (!statusEffects.Contains(effect)) return;

	statusEffects.Remove(effect);
}
void FUnitData::ClearStatusEffects()
{
	statusEffects.Empty();
}

TMap<UnitTypes, FUnitComposition> FUnitData::GetUnitComposition() const
{
	TMap<UnitTypes, FUnitComposition> units;

	units.Add(UnitTypes::Infantry, FUnitComposition{});
	units.Add(UnitTypes::Cavalry, FUnitComposition{});
	units.Add(UnitTypes::Scout, FUnitComposition{});
	units.Add(UnitTypes::Ranged, FUnitComposition{});
	units.Add(UnitTypes::Shielder, FUnitComposition{});
	units.Add(UnitTypes::Settler, FUnitComposition{});

	if (unitType != UnitTypes::Army)
	{
		units[unitType].quantity++;
		units[unitType].compPercent = 1.f;

		return units;
	}

	int totalUnits = 0;

	for (int i = 0; i < savedUnits.Num(); i++)
	{
		++units[savedUnits[i]->unitType].quantity;
		++totalUnits;
	}

	for (auto& unit : units)
	{
		unit.Value.compPercent = (float)unit.Value.quantity / (float)totalUnits;
	}

	return units;
}
UnitTypes FUnitData::GetLargestUnitQuantity() const
{
	if (unitType != UnitTypes::Army) return unitType;

	TMap<UnitTypes, FUnitComposition> composition = GetUnitComposition();

	UnitTypes highestType = UnitTypes::None;
	int best = 0;

	for (auto& unit : composition)
	{
		if (unit.Value.quantity > best)
		{
			best = unit.Value.quantity;
			highestType = unit.Key;
		}
	}

	return highestType;
}
#pragma endregion

#pragma region Status Effects
FString FStatusEffect::GetEffectName() const
{
	return effectName;
}
Factions FStatusEffect::GetOriginFaction() const
{
	return originFaction;
}
FactionRelationship FStatusEffect::GetFactionsToAffect() const
{
	return factionsToAffect;
}
float FStatusEffect::GetHPMod() const
{
	return hpMod;
}
float FStatusEffect::GetMoraleMod() const
{
	return moraleMod;
}
int FStatusEffect::GetVisionMod() const
{
	return visionMod;
}
float FStatusEffect::GetSpeedMod() const
{
	return speedMod;
}
float FStatusEffect::GetDamageMod() const
{
	return damageMod;
}
float FStatusEffect::GetSiegePowerMod() const
{
	return siegePowerMod;
}
#pragma endregion

#pragma region Point of Interest
FString FPointOfInterest::GetPointTitle() const
{
	return pointTitle;
}
const TMap<EStratResources, int32>& FPointOfInterest::GetRewards() const
{
	return rewards;
}
FString FPointOfInterest::GetWorldDisplay() const
{
	FString message = pointTitle + FString::Printf(TEXT(" - <img id=\":time:\"/> %d day%s\n"), daysRemaining,
		daysRemaining > 1 ? TEXT("s") : TEXT(""));

	for (const TPair<EStratResources, int32>& r : rewards)
	{
		FString resource = "";
		switch (r.Key)
		{
		case EStratResources::Energy:
			resource += TEXT(":energy:");
			break;
		case EStratResources::Production:
			resource += TEXT(":prod:");
			break;
		case EStratResources::Food:
			resource += TEXT(":food:");
			break;
		case EStratResources::Wealth:
			resource += TEXT(":wealth:");
			break;
		case EStratResources::Population:
			resource += TEXT(":pop:");
			break;
		}

		message += FString::Printf(TEXT("<img id=\"%s\"/>%d "), *resource, r.Value);
	}

	return message;
}
int32 FPointOfInterest::GetDaysToComplete() const
{
	return daysRemaining;
}
bool FPointOfInterest::Work()
{
	if (WorkCompleted()) return false;

	if (!ACapstoneProjectGameModeBase::GetDateUpdates()->minuteTick) return false;

	minutes++;
	if (minutes < 2) return false;

	hoursRemaining++;
	minutes = 0;
	if (hoursRemaining < 24) return false;

	daysRemaining--;
	hoursRemaining = 0;

	return WorkCompleted();
}
bool FPointOfInterest::WorkCompleted() const
{
	return daysRemaining <= 0;
}
float FPointOfInterest::GetWorkProgress() const
{
	long int daysToMinutes = (daysToComplete - daysRemaining) * 1440;
	long int hoursToMinutes = hoursRemaining * 60;

	long int totalMinutes = daysToComplete * 1440;
	long int currentMinutes = minutes + hoursToMinutes + daysToMinutes;
	
	return static_cast<float>(currentMinutes) / static_cast<float>(totalMinutes);
}
#pragma endregion