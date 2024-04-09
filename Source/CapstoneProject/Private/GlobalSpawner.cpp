// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalSpawner.h"
#include "BaseHex.h"
#include "HexNav.h"
#include "Building.h"
#include "Troop.h"
#include "Settler.h"
#include "MergedArmy.h"
#include "MiningStation.h"
#include "Farmland.h"
#include "PowerPlant.h"
#include "Outpost.h"
#include "CapitalHub.h"
#include "AlienCity.h"
#include "RockCity.h"
#include "BuildingAttachment.h"
#include "BattleObject.h"
#include "SiegeObject.h"

// Sets default values
AGlobalSpawner::AGlobalSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	buildingCosts.Add(SpawnableBuildings::MiningStation, FBuildingCost{ 150, 10, 5, FText::FromString("Mining Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Production_Station.Production_Station'")) });
	buildingCosts.Add(SpawnableBuildings::Farmland, FBuildingCost{ 150, 10, 5, FText::FromString("Farmland"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Food_Farming_Station.Food_Farming_Station'"))});
	buildingCosts.Add(SpawnableBuildings::PowerPlant, FBuildingCost{ 150, 10, 5, FText::FromString("Power Plant"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Energy_Station.Energy_Station'")) });
	buildingCosts.Add(SpawnableBuildings::Outpost, FBuildingCost{ 0, 10, 10, FText::FromString("Outpost"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'")) });
	buildingCosts.Add(SpawnableBuildings::Capitol, FBuildingCost{0, 0, 0, FText::FromString("Capitol Hub"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Trade_Outpost_Station.Building_Icon_Trade_Outpost_Station'")) });

	attachmentCosts.Add(BuildingAttachments::Storage, FBuildingCost{ 75, 5, 48, FText::FromString("Storage"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Storage.Building_Icon_Robot_Storage'")) });
	attachmentCosts.Add(BuildingAttachments::DefenseStation, FBuildingCost{ 75, 5, 48, FText::FromString("Defense Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Factory.Building_Icon_Robot_Factory'")) });
	attachmentCosts.Add(BuildingAttachments::RobotFactory, FBuildingCost{ 75, 5, 48, FText::FromString("Robot Factory"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'")) });
	attachmentCosts.Add(BuildingAttachments::RobotBarracks, FBuildingCost{ 75, 5, 48, FText::FromString("Robot Barracks"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Material_Storage.Building_Icon_Material_Storage'")) });

	attachmentCosts.Add(BuildingAttachments::TradeOutpost, FBuildingCost{ 50, 5, 48 });
	attachmentCosts.Add(BuildingAttachments::Embassy, FBuildingCost{ 50, 5, 48 });
	attachmentCosts.Add(BuildingAttachments::PoliceStation, FBuildingCost{ 100, 10, 96 });

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

	//Infantry Unit Stats
	troopStats.Add(UnitTypes::Infantry, FTroopStats{ FText::FromString("Infantry"), FText::FromString("Standard robots equiped with melee fuctionality and a short ranged scatterlaser."), UnitTypes::Infantry, 240, 15, 16, 12, 1, 100, 1, 1,
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
	troopStats.Add(UnitTypes::Cavalry, FTroopStats{ FText::FromString("Cavalry"), FText::FromString("Streamlined robots riding hoverbikes that are equiped with titanium tipped lances and laser pistols."), UnitTypes::Cavalry, 180, 11, 20, 8, 1, 100, 1, 1,
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
	troopStats.Add(UnitTypes::Ranged, FTroopStats{ FText::FromString("Ranged"), FText::FromString("Lighty armored robots that use long ranged laser rifles to devastating effect."), UnitTypes::Ranged, 160, 20, 12, 20, 2, 100, 1, 1,
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
	troopStats.Add(UnitTypes::Shielder, FTroopStats{ FText::FromString("Shielder"), FText::FromString("Heavily armored robots with thick riot shields and short ranged laser weaponry."), UnitTypes::Shielder, 280, 19, 8, 16, 1, 100, 1, 1,
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
	troopStats.Add(UnitTypes::Scout, FTroopStats{ FText::FromString("Scout"), FText::FromString("Lightly armored quick robots with enhanced visual capabilities."), UnitTypes::Scout, 140, 7, 4, 4, 3, 100, 1, 1,
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
	troopStats.Add(UnitTypes::Settler, FTroopStats{ FText::FromString("Settler"), FText::FromString("A colony on legs, settlers carry materials and workers to create new outposts."), UnitTypes::Settler, 200, 15, 0, 0, 1, 100, 1, 1,
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

	//Mining Station Stats
	buildingStats.Add(SpawnableBuildings::MiningStation, FBuildingStats{ FText::FromString("Mining Station"), FText::FromString("A drilling station that extracts resources necessary for building troops and new facilities."),
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
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Production_Station.Production_Station'"))
		});
	//Farm Stats
	buildingStats.Add(SpawnableBuildings::Farmland, FBuildingStats{ FText::FromString("Farmland"), FText::FromString("Plots of land dedicated to provided the colony with food or cash crops."),
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
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Food_Farming_Station.Food_Farming_Station'"))
		});
	//Power Plant Stats
	buildingStats.Add(SpawnableBuildings::PowerPlant, FBuildingStats{ FText::FromString("Power Plant"), FText::FromString("A rudamentory fission reactor that produces energy to power troops and facilities."),
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
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Energy_Station.Energy_Station'"))
		});
	//Outpost Stats
	buildingStats.Add(SpawnableBuildings::Outpost, FBuildingStats{ FText::FromString("Outpost"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited."),
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
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		5,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'"))
		});
	//Capitol Hub
	buildingStats.Add(SpawnableBuildings::Capitol, FBuildingStats{ FText::FromString("Capitol Hub"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited."),
		/*Energy Yield*/
		4,
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
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		7,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'"))
		});
	buildingStats.Add(SpawnableBuildings::AlienCity, FBuildingStats{ FText::FromString("Alien City"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited."),
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
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		5,
		/*Max Workers*/
		10,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'"))
		});
	buildingStats.Add(SpawnableBuildings::RockCity, FBuildingStats{ FText::FromString("Rock City"), FText::FromString("A central location that expands the colony's zone of control, allowing for additional tiles to be exploited."),
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
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		5,
		/*Max Workers*/
		15,
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'"))
		});
	//Material Storage Stats
	attachmentStats.Add(BuildingAttachments::Storage, FBuildingStats{ FText::FromString("Material Storage"), FText::FromString("A warehouse extension that provides additional storage capacity for the colony's resources."),
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
		0.f,
		/*Trade*/
		0.f,
		/*Damage*/
		0,
		/*HP*/
		0,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		5
		});
	//Defense Station Stats
	attachmentStats.Add(BuildingAttachments::DefenseStation, FBuildingStats{ FText::FromString("Defense Station"), FText::FromString("A fortress extension that greatly enhances defensive capabilities."),
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
		0.f,
		/*Trade*/
		0.f,
		/*Damage*/
		2,
		/*HP*/
		200,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		5
		});
	//Robot Factory Stats
	attachmentStats.Add(BuildingAttachments::RobotFactory, FBuildingStats{ FText::FromString("Robot Factory"), FText::FromString("A highly industrialized factory capable of creating robotic armies."),
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
		0.f,
		/*Trade*/
		0.f,
		/*Damage*/
		0,
		/*HP*/
		0,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		5
		});
	//Robot Barracks Stats
	attachmentStats.Add(BuildingAttachments::RobotBarracks, FBuildingStats{ FText::FromString("Robot Barracks"), FText::FromString("An extension that allows for robot units to be repaired when on the tile."),
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
		100,
		/*Diplomacy*/
		0.f,
		/*Trade*/
		0.f,
		/*Damage*/
		0,
		/*HP*/
		0,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		5
		});
	//Trade Outpost Stats
	attachmentStats.Add(BuildingAttachments::TradeOutpost, FBuildingStats{ FText::FromString("Trade Outpost"), FText::FromString("A trading entrepot that allows for more favorable trading terms."),
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
		0.f,
		/*Trade*/
		1.15f,
		/*Damage*/
		0,
		/*HP*/
		0,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		5
		});
	//Embassy Stats
	attachmentStats.Add(BuildingAttachments::Embassy, FBuildingStats{ FText::FromString("Embassy"), FText::FromString("An embassy helps establish more favorable diplomatic ties with alien civilizations."),
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
		1.15f,
		/*Trade*/
		0.f,
		/*Damage*/
		0,
		/*HP*/
		5,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		5
		});
	//Police Station Stats
	attachmentStats.Add(BuildingAttachments::PoliceStation, FBuildingStats{ FText::FromString("Police Station"), FText::FromString("The police station decreases alien unrest and prevents the need for robotic armies to remain in the city to suppress revolts."),
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
		0.f,
		/*Trade*/
		0.f,
		/*Damage*/
		0,
		/*HP*/
		10,
		/*Unrest*/
		-0.85f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		10
		});

	terrainTileMaterials.Add(TerrainType::None, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/UNDISCOVERED")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/UNDISCOVERED")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/UNDISCOVERED")) });
	terrainTileMaterials.Add(TerrainType::Plains, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainPlainsMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainPlainsMat01")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainPlainsMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::Hills, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainHillMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainHillMat01")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainHillMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::Mountains, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainMntMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainMntMat01")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainMntMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::SporeField, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainToxicMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainToxicMat01")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainToxicMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::Forest, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TileForestFloor_Mat_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileForestFloor_Mat")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TileForestFloor_Mat_HL")),

		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainForestMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainForestMat01")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainForestMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::Jungle, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TileForestHeavyFloor_Mat_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileForestHeavyFloor_Mat")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TileForestHeavyFloor_Mat_HL")),

		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainJungleMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainJungleMat01")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainJungleMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::Ship, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainPlainsMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TilePlainsFloor_Mat")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainPlainsMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::AlienCity, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainPlainsMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TilePlainsFloor_Mat")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainPlainsMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::TheRock, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainPlainsMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TilePlainsFloor_Mat")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainPlainsMat01_HL")) });
	terrainTileMaterials.Add(TerrainType::Border, FVisibilityMaterials{
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainPlainsMat01_Hidden")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TilePlainsFloor_Mat")),
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainPlainsMat01_HL")) });

	troopFactionMaterials.Add(Factions::Human, FTroopMaterials{ LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HumanTroopMat01")), 
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/HLHumanTroopMat")) });
	troopFactionMaterials.Add(Factions::Alien1, FTroopMaterials{ LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/BuildingInDevMat01")), 
		LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/HLHumanTroopMat")) });
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
	if (!capitalPrefab) capitalPrefab = ACapitalHub::StaticClass();
	if (!battlePrefab) battlePrefab = ABattleObject::StaticClass();
	if (!siegePrefab) siegePrefab = ASiegeObject::StaticClass();
	if (!alienCityPrefab) alienCityPrefab = AAlienCity::StaticClass();
	if (!rockCityPrefab) rockCityPrefab = ARockCity::StaticClass();

	ProceduralHexGen(400, ShapesOfMap::Square);
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
	case SpawnableBuildings::Capitol:
		return capitalPrefab;
	default:
		return nullptr;
	}
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
		mergedArmy = GetWorld()->SpawnActor<AMergedArmy>(mergedArmyPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);

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
		testSeeker->ConsumeArmy(testTarget);
		break;
	}
}

void AGlobalSpawner::CreateHexModel(TerrainType terrainType, ABaseHex* hex)
{
	UStaticMesh* meshAsset = nullptr;
	UStaticMesh* extraAsset = nullptr;

	ABuilding* newBuilding = nullptr;

	if (terrainTileMaterials.Contains(terrainType))
	{
		FVisibilityMaterials materials = terrainTileMaterials[terrainType];

		if (materials.visibleTexture) hex->visibility->meshMaterials.visibleTexture = materials.visibleTexture;
		if (materials.hiddenTexture) hex->visibility->meshMaterials.hiddenTexture = materials.hiddenTexture;
		if (materials.selectedTexture) hex->visibility->meshMaterials.selectedTexture = materials.selectedTexture;

		if (materials.modelVisibleTexture) hex->visibility->meshMaterials.modelVisibleTexture = materials.modelVisibleTexture;
		if (materials.modelHiddenTexture) hex->visibility->meshMaterials.modelHiddenTexture = materials.modelHiddenTexture;
		if (materials.modelSelectedTexture) hex->visibility->meshMaterials.modelSelectedTexture = materials.modelSelectedTexture;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("TerrainType not found"));
	}

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
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_Hexagon.TileJungleModel_Hexagon'"));
		break;
	case TerrainType::Ship:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForestModel_HexagonPlains.TileForestModel_HexagonPlains'"));
		extraAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingCapitolShip2.BuildingCapitolShip2'"));

		break;
	case TerrainType::AlienCity:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_Hexagon.TileJungleModel_Hexagon'"));

		newBuilding = GetWorld()->SpawnActor<ABuilding>(alienCityPrefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
		UnitActions::AssignFaction(Factions::Alien1, newBuilding);

		break;
	case TerrainType::TheRock:		
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_Hexagon.TileJungleModel_Hexagon'"));

		newBuilding = GetWorld()->SpawnActor<ABuilding>(rockCityPrefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
		UnitActions::AssignFaction(Factions::Alien1, newBuilding);

		break;
	default:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/CyTheHexGuy.CyTheHexGuy'"));
		break;
	}

	if (meshAsset)
	{
		hex->hexMesh->SetStaticMesh(meshAsset);
	}

	if (extraAsset)
	{
		hex->hexMeshAttachment->SetStaticMesh(extraAsset);
		if (terrainType == TerrainType::Ship)
		{
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, -45.f, 0));
		}
		else
		{
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, 0, 0));
		}
	}

	hex->hexMeshAttachment->SetVisibility(extraAsset != nullptr);
	hex->attachmentCanBeVisible = extraAsset != nullptr;
}

void AGlobalSpawner::ProceduralHexGen(int numHexs, ShapesOfMap shape)
{
	bool shipExists = false;
	bool rockExists = false;
	float stepDistanceX = 63.f;
	float stepDistanceY = -73.5f;
	FVector origin = FVector(-250.f, 400.f, -1.f);
	float sqroot;
	int roundedSQRoot;
	ABaseHex* newHex;

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
				float oddHex = (x % 2 > 0) ? 1 : 0;

				FVector spawnPos = origin + FVector(stepDistanceX * x, (stepDistanceY * y) + (oddHex * stepDistanceY / 2), 0.f);

				newHex = GetWorld()->SpawnActor<ABaseHex>(hexActor, spawnPos, FRotator::ZeroRotator);

				newHex->terrainChange = TerrainType(FMath::RandRange(1, 6));

				column.Add(newHex);
			}

			arrayOfHexColumns.Add(column);
		}

		hexArray = arrayOfHexColumns;

		hexArray[3][4]->terrainChange = TerrainType::Ship;

		hexArray[FMath::RandRange(1, 19)][FMath::RandRange(6, 19)]->terrainChange = TerrainType::TheRock;

		
		for (int i = 0; i < 6; i++)
		{
			ABaseHex* hex = nullptr;
			FVector2D randomXY = FVector2D::Zero();

			while (!hex)
			{
				randomXY = FVector2D(FMath::RandRange(1, 19), FMath::RandRange(5, 19));
				if (!usedCoordinates.Contains(randomXY))
				{
					ABaseHex* hexTest = hexArray[randomXY.X][randomXY.Y];
					if (hexTest->terrainChange != TerrainType::AlienCity &&
						hexTest->terrainChange != TerrainType::TheRock &&
						hexTest->terrainChange != TerrainType::Ship &&
						!BuildingOnHex(hexTest))
					{
						usedCoordinates.Add(randomXY);
						hex = hexTest;
					}
				}
			}

			hex->terrainChange = TerrainType::AlienCity;
			SpawnBuildingsAroundCity(hex);
		}

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

FVector2D AGlobalSpawner::GetHexCoordinates(ABaseHex* hex)
{
	int hexesSearched = 0;
	for (int x = 0; x < hexArray.Num(); x++)
	{
		for (int y = 0; y < hexArray[x].Num(); y++)
		{
			hexesSearched++;
			if (hex == hexArray[x][y])
			{
				return FVector2D(x, y);
			}
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("%d hexes searched"), hexesSearched));

	return FVector2D::Zero();
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

void AGlobalSpawner::SpawnBuildingsAroundCity(ABaseHex* centerHex)
{
	FVector2D center = GetHexCoordinates(centerHex);
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

				if (hexTest->terrainChange != TerrainType::AlienCity && 
					hexTest->terrainChange != TerrainType::TheRock && 
					hexTest->terrainChange != TerrainType::Ship &&
					!BuildingOnHex(hexTest))
				{
					hex = hexTest;
				}
				
			}
		}

		randomHexes.Add(hex);
		claimedCoordinates.Add(FVector2D(randX, randY));
	}

	ABuilding* miningStation = GetWorld()->SpawnActor<ABuilding>(miningStationPrefab, randomHexes[0]->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	UnitActions::AssignFaction(Factions::Alien1, miningStation);
	miningStation->builtAtStart = true;

	ABuilding* farmland = GetWorld()->SpawnActor<ABuilding>(farmlandPrefab, randomHexes[1]->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	UnitActions::AssignFaction(Factions::Alien1, farmland);
	farmland->builtAtStart = true;

	ABuilding* powerPlant = GetWorld()->SpawnActor<ABuilding>(powerPlantPrefab, randomHexes[2]->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	UnitActions::AssignFaction(Factions::Alien1, powerPlant);
	powerPlant->builtAtStart = true;
}

void AGlobalSpawner::SpawnBuilding(Factions faction, SpawnableBuildings building, ABaseHex* hex)
{
	//If hex already
	if (hex->building)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Hex already occupied"));
	}

	//Determine building prefab to spawn
	FActorSpawnParameters params;
	UClass* prefab = DetermineBuildingType(building);

	//If no prefab found, return
	if (!prefab) return;

	//Declare bool for whether this building is affordable
	bool canAfford = false;

	//Declare TMaps for resources and workers to spend on this building
	TMap<StratResources, int> resourceCosts = TMap<StratResources, int>();
	AMovementAI* settlerOnHex = nullptr;

	//If the desired building exists in the 
	if (buildingCosts.Contains(building))
	{
		TMap<StratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);

		if (resources[StratResources::Production] >= buildingCosts[building].productionCost)
		{
			if (building == SpawnableBuildings::Outpost)
			{
				for (int i = 0; i < hex->troopsInHex.Num(); i++)
				{
					if (hex->troopsInHex[i]->unitStats->unitType == UnitTypes::Settler)
					{
						if (hex->troopsInHex[i]->unitStats->faction == faction)
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

		resourceCosts.Add(StratResources::Production, buildingCosts[building].productionCost);
	}

	if (canAfford)
	{
		ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(prefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
		UnitActions::ConsumeSpentResources(faction, resourceCosts, hex);
		UnitActions::AssignFaction(faction, newBuilding);

		if (building == SpawnableBuildings::Outpost)
		{
			UnitActions::SetWorkers(faction, WorkerType::Human, troopCosts[UnitTypes::Settler].populationCost);
			if (settlerOnHex) settlerOnHex->Destroy();
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Cannot afford building"));
	}
}

ATroop* AGlobalSpawner::SpawnTroop(ABaseHex* hex, UnitActions::UnitData data, float parentHealthPercent)
{
	FActorSpawnParameters params;
	if (!troopPrefab) return nullptr;

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);

	float result = (float)data.currentHP * parentHealthPercent;
	if (result < 1.f) result = 1.f;
	data.currentHP = result;

	UnitActions::ApplyDataToUnitStats(newTroop->unitStats, data);

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
	ABattleObject* battle = GetWorld()->SpawnActor<ABattleObject>(hex->building ? siegePrefab : battlePrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0.f, 0.f, 0.f), params);
	battle->hexNav->currentHex = hex;
	battle->spawner = this;
	hex->battleInProgress = true;
	battle->Start();

	return battle;
}

AActor* AGlobalSpawner::SpawnSmoke(AActor* object)
{
	if (!smokePrefab) return nullptr;

	FActorSpawnParameters params;
	AActor* particleSystem = GetWorld()->SpawnActor<AActor>(smokePrefab, object->GetActorLocation(), FRotator(0.f, 0.f, 0.f), params);

	return particleSystem;
}

bool AGlobalSpawner::PurchaseTroop(Factions faction, UnitTypes unit, AOutpost* outpost)
{
	bool canAfford = false;

	TMap<StratResources, int> resourceCosts = TMap<StratResources, int>();
	if (troopCosts.Contains(unit))
	{
		TMap<StratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);

		if (resources[StratResources::Production] >= troopCosts[unit].productionCost)
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

		resourceCosts.Add(StratResources::Production, troopCosts[unit].productionCost);
	}

	if (canAfford) UnitActions::ConsumeSpentResources(faction, resourceCosts, nullptr);
	

	return canAfford;
}

ATroop* AGlobalSpawner::BuildTroop(Factions faction, UnitTypes unit, ABaseHex* hex)
{
	FTroopStats unitData = troopStats[unit];	

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	UnitActions::AssignFaction(faction, newTroop);
	UnitActions::ApplyDataToUnitStats(newTroop->unitStats, unitData);

	return newTroop;
}

void AGlobalSpawner::BuildAttachment(Factions faction, BuildingAttachments attachment, AOutpost* outpost)
{
	if (faction == Factions::None) return;

	UBuildingAttachment* selectedAttachment = outpost->GetAttachment(attachment);

	bool canAfford = false;

	TMap<StratResources, int> resourceCosts = TMap<StratResources, int>();
	TMap<WorkerType, int> workerCosts = TMap<WorkerType, int>();

	if (!attachmentCosts.Contains(attachment)) return;

	TMap<StratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);
	TMap<WorkerType, int> workers = UnitActions::GetFactionWorkers(faction);

	if (resources[StratResources::Production] >= attachmentCosts[attachment].workerCost && workers[WorkerType::Human] > attachmentCosts[attachment].workerCost)
	{
		canAfford = true;
	}

	resourceCosts.Add(StratResources::Production, attachmentCosts[attachment].productionCost);
	workerCosts.Add(WorkerType::Human, attachmentCosts[attachment].workerCost - selectedAttachment->GetNumberOfWorkers());

	if (canAfford)
	{
		outpost->BuildAttachment(attachment);
		UnitActions::ConsumeSpentResources(faction, resourceCosts, workerCosts, outpost, attachment);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Cannot afford attachment"));
	}
}

