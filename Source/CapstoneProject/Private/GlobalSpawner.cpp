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
#include "BuildingAttachment.h"
#include "BattleObject.h"

// Sets default values
AGlobalSpawner::AGlobalSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	buildingCosts.Add(SpawnableBuildings::MiningStation, FBuildingCost{ 200, 30, 5, FText::FromString("Mining Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Production_Station.Production_Station'")) });
	buildingCosts.Add(SpawnableBuildings::Farmland, FBuildingCost{ 100, 30, 5, FText::FromString("Farmland"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Food_Farming_Station.Food_Farming_Station'"))});
	buildingCosts.Add(SpawnableBuildings::PowerPlant, FBuildingCost{ 200, 30, 5, FText::FromString("Power Plant"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Energy_Station.Energy_Station'")) });
	buildingCosts.Add(SpawnableBuildings::Outpost, FBuildingCost{ 0, 0, 10, FText::FromString("Outpost"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost.Building_Icon_Outpost'")) });

	attachmentCosts.Add(BuildingAttachments::Storage, FBuildingCost{ 100, 15, 120, FText::FromString("Storage"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Storage'")) });
	attachmentCosts.Add(BuildingAttachments::DefenseStation, FBuildingCost{ 100, 20, 120, FText::FromString("Defense Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Robot_Factory'")) });
	attachmentCosts.Add(BuildingAttachments::RobotFactory, FBuildingCost{ 100, 30, 150, FText::FromString("Robot Factory"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Outpost'")) });
	attachmentCosts.Add(BuildingAttachments::RobotBarracks, FBuildingCost{ 100, 15, 120, FText::FromString("Robot Barracks"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Building_Icon_Material_Storage'")) });

	attachmentCosts.Add(BuildingAttachments::TradeOutpost, FBuildingCost{ 50, 5, 80 });
	attachmentCosts.Add(BuildingAttachments::Embassy, FBuildingCost{ 50, 5, 100 });
	attachmentCosts.Add(BuildingAttachments::PoliceStation, FBuildingCost{ 100, 40, 120 });

	troopCosts.Add(UnitTypes::Infantry, FTroopCost{ 100, 48, 0, FText::FromString("Infantry"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Infantry_Nail.Infantry_Nail'")) });
	troopCosts.Add(UnitTypes::Cavalry, FTroopCost{ 100, 60, 0, FText::FromString("Cavalry"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Cavalry_Nail.Cavalry_Nail'")) });
	troopCosts.Add(UnitTypes::Ranged, FTroopCost{ 100, 48, 0, FText::FromString("Ranged"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Ranged_Nail.Ranged_Nail'")) });
	troopCosts.Add(UnitTypes::Shielder, FTroopCost{ 100, 60, 0, FText::FromString("Shielder"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Shielder_Nail.Shielder_Nail'")) });
	troopCosts.Add(UnitTypes::Scout, FTroopCost{ 20, 24, 0, FText::FromString("Scout"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Scout_Nail.Scout_Nail'")) });
	troopCosts.Add(UnitTypes::Settler, FTroopCost{ 400, 60, 50, FText::FromString("Settler"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/Battle_Icons_Nails/Settler_Nail.Settler_Nail'")) });

	//Infantry Unit Stats
	troopStats.Add(UnitTypes::Infantry, FTroopStats{ FText::FromString("Infantry"), FText::FromString("Standard robots equiped with melee fuctionality and a short ranged scatterlaser."), UnitTypes::Infantry, 100, 60, 6, 10, 1, 100, 1, 1,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 50,
		/*Ranged*/ 100,
		/*Shielder*/ 200,
		/*Scout*/ 100, 
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 200,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Cavalry Unit Stats
	troopStats.Add(UnitTypes::Cavalry, FTroopStats{ FText::FromString("Cavalry"), FText::FromString("Streamlined robots riding hoverbikes that are equiped with titanium tipped lances and laser pistols."), UnitTypes::Cavalry, 120, 45, 8, 5, 1, 100, 1, 1,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 200,
		/*Shielder*/ 50,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 50,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Ranged Unit Stats
	troopStats.Add(UnitTypes::Ranged, FTroopStats{ FText::FromString("Ranged"), FText::FromString("Lighty armored robots that use long ranged laser rifles to devastating effect."), UnitTypes::Ranged, 80, 80, 4, 20, 2, 100, 1, 1,
		/*** Attacking ***/
		/*Infantry*/ 200,
		/*Cavalry*/ 100,
		/*Ranged*/ 100,
		/*Shielder*/ 50,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 200,
		/*Ranged*/ 100,
		/*Shielder*/ 50,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Shielder Unit Stats
	troopStats.Add(UnitTypes::Shielder, FTroopStats{ FText::FromString("Shielder"), FText::FromString("Heavily armored robots with thick riot shields and short ranged laser weaponry."), UnitTypes::Shielder, 150, 75, 3, 15, 1, 100, 1, 1,
		/*** Attacking ***/
		/*Infantry*/ 100,
		/*Cavalry*/ 100,
		/*Ranged*/ 50,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100,

		/*** Defending ***/
		/*Infantry*/ 200,
		/*Cavalry*/ 50,
		/*Ranged*/ 50,
		/*Shielder*/ 100,
		/*Scout*/ 100,
		/*Settler*/ 100
		});
	//Scout Unit Stats
	troopStats.Add(UnitTypes::Scout, FTroopStats{ FText::FromString("Scout"), FText::FromString("Lightly armored quick robots with enhanced visual capabilities."), UnitTypes::Scout, 50, 30, 2, 1, 3, 100, 1, 1,
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
	troopStats.Add(UnitTypes::Settler, FTroopStats{ FText::FromString("Settler"), FText::FromString("A colony on legs, settlers carry materials and workers to create new outposts."), UnitTypes::Settler, 75, 60, 0, 0, 1, 100, 1, 1,
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
		5,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		30
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
		5,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		30
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
		5,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		30
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
		100,
		/*Diplomacy*/
		0,
		/*Trade*/
		0,
		/*Damage*/
		7,
		/*HP*/
		15,
		/*Unrest*/
		0,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		50
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
		5,
		/*HP*/
		10,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		15
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
		10,
		/*HP*/
		20,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		20
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
		5,
		/*HP*/
		10,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		30
		});
	//Robot Factory Stats
	attachmentStats.Add(BuildingAttachments::RobotBarracks, FBuildingStats{ FText::FromString("Robot Barracks"), FText::FromString("An extension that allows for robot units to be stored safely without powering them."),
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
		1000,
		/*Diplomacy*/
		0.f,
		/*Trade*/
		0.f,
		/*Damage*/
		5,
		/*HP*/
		20,
		/*Unrest*/
		0.f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		15
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
		2,
		/*HP*/
		5,
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
		2,
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
		5,
		/*HP*/
		10,
		/*Unrest*/
		-0.85f,
		/*Energy Upkeep Cost*/
		2,
		/*Max Workers*/
		40
		});
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
	if (!alienCityPrefab) alienCityPrefab = AAlienCity::StaticClass();

	ProceduralHexGen(200, ShapesOfMap::Square);
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

	UMaterialInterface* matAssetVisible = nullptr;
	UMaterialInterface* matAssetHidden = nullptr;
	UMaterialInterface* extraMatAssetVisible = nullptr;
	UMaterialInterface* extraMatAssetHidden = nullptr;

	UMaterialInterface* matAssetSelected = nullptr;
	UMaterialInterface* extraMatAssetSelected = nullptr;

	ABuilding* newBuilding = nullptr;

	switch (terrainType)
	{
	case TerrainType::None:
		hex->hexMeshAttachment->SetVisibility(false);
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TilePlains.TilePlains'"));

		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/UNDISCOVERED"));
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/UNDISCOVERED"));
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/UNDISCOVERED"));
		break;
	case TerrainType::Plains:
		hex->hexMeshAttachment->SetVisibility(false);
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TilePlains.TilePlains'"));

		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TilePlainsFloor_Mat"));		
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainPlainsMat01_Hidden"));	
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainPlainsMat01_HL"));
		break;
	case TerrainType::Hills:
		hex->hexMeshAttachment->SetVisibility(false);
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileHills.TileHills'"));
		
		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainHillMat01"));		
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainHillMat01_Hidden"));	
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainHillMat01_HL"));
		break;
	case TerrainType::Mountains:
		hex->hexMeshAttachment->SetVisibility(false);
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileMountainModel.TileMountainModel'"));

		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainMntMat01"));		
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainMntMat01_Hidden"));
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainMntMat01_HL"));
		break;
	case TerrainType::Forest:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForest.TileForest'"));
		extraAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForestModel_TileForest.TileForestModel_TileForest'"));

		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileForestFloor_Mat"));
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TileForestFloor_Mat_Hidden"));
		extraMatAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainForestMat01"));
		extraMatAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainForestMat01_Hidden"));
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TileForestFloor_Mat_HL"));
		extraMatAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainForestMat01_HL"));
		break;
	case TerrainType::Jungle:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'"));
		extraAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_TileForestThick.TileJungleModel_TileForestThick'"));

		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileForestHeavyFloor_Mat"));
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TileForestHeavyFloor_Mat_Hidden"));
		extraMatAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainJungleMat01"));
		extraMatAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainJungleMat01_Hidden"));
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TileForestHeavyFloor_Mat_HL"));
		extraMatAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainJungleMat01_HL"));
		break;
	case TerrainType::SporeField:
		hex->hexMeshAttachment->SetVisibility(false);
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileSporefieldModel.TileSporefieldModel'"));

		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TerrainToxicMat01"));
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TerrainToxicMat01_Hidden"));
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TerrainToxicMat01_HL"));
		break;
	case TerrainType::Border:
		hex->hexMeshAttachment->SetVisibility(false);
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_Hexagon.TileJungleModel_Hexagon'"));
		break;
	case TerrainType::Ship:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForestModel_HexagonPlains.TileForestModel_HexagonPlains'"));

		newBuilding = GetWorld()->SpawnActor<ABuilding>(capitalPrefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
		UnitActions::AssignFaction(Factions::Human, newBuilding);

		matAssetVisible = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/TileForestHeavyFloor_Mat"));
		matAssetHidden = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HiddenVersions/TileForestHeavyFloor_Mat_Hidden"));
		matAssetSelected = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/HighlightedVersions/TileForestHeavyFloor_Mat_HL"));
		break;
	case TerrainType::AlienCity:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_Hexagon.TileJungleModel_Hexagon'"));

		newBuilding = GetWorld()->SpawnActor<ABuilding>(alienCityPrefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0));
		UnitActions::AssignFaction(Factions::Alien1, newBuilding);
		break;
	case TerrainType::TheRock:
		meshAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_Hexagon.TileJungleModel_Hexagon'"));
		extraAsset = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingBigRockCity.BuildingBigRockCity'"));
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
		hex->hexMeshAttachment->SetVisibility(true);
	}
	if (matAssetVisible)
	{
		hex->visibility->meshMaterials.visibleTexture = matAssetVisible;
	}
	if (matAssetHidden)
	{
		hex->visibility->meshMaterials.hiddenTexture = matAssetHidden;
	}
	if (extraMatAssetVisible)
	{
		hex->visibility->otherMeshMaterials.visibleTexture = extraMatAssetVisible;
	}
	if (extraMatAssetHidden)
	{
		hex->visibility->otherMeshMaterials.hiddenTexture = extraMatAssetHidden;
	}
	if (matAssetSelected)
	{
		hex->visibility->meshMaterials.selectedTexture = matAssetSelected;
	}
	if (extraMatAssetSelected)
	{
		hex->visibility->otherMeshMaterials.selectedTexture = extraMatAssetSelected;
	}
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

				column.Add(newHex);
			}

			arrayOfHexColumns.Add(column);
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
	TMap<WorkerType, int> workerCosts = TMap<WorkerType, int>();

	//If the desired building exists in the 
	if (buildingCosts.Contains(building))
	{
		TMap<StratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);
		TMap<WorkerType, int> workers = UnitActions::GetFactionWorkers(faction);

		if (resources[StratResources::Production] >= buildingCosts[building].productionCost && workers[WorkerType::Human] > buildingCosts[building].workerCost)
		{
			canAfford = true;
		}

		resourceCosts.Add(StratResources::Production, buildingCosts[building].productionCost);
		workerCosts.Add(WorkerType::Human, buildingCosts[building].workerCost - hex->GetNumberOfWorkers());
	}

	if (canAfford)
	{
		ABuilding* newBuilding = GetWorld()->SpawnActor<ABuilding>(prefab, hex->buildingAnchor->GetComponentLocation(), FRotator(0, 0, 0), params);
		UnitActions::ConsumeSpentResources(faction, resourceCosts, workerCosts, hex);
		UnitActions::AssignFaction(faction, newBuilding);
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
	ABattleObject* battle = GetWorld()->SpawnActor<ABattleObject>(battlePrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0.f, 0.f, 0.f), params);
	battle->hexNav->currentHex = hex;
	battle->spawner = this;
	hex->battleInProgress = true;
	battle->Start();

	return battle;
}

bool AGlobalSpawner::PurchaseTroop(Factions faction, UnitTypes unit, AOutpost* outpost)
{
	bool canAfford = false;

	TMap<StratResources, int> resourceCosts = TMap<StratResources, int>();
	TMap<WorkerType, int> popCosts = TMap<WorkerType, int>();
	if (troopCosts.Contains(unit))
	{
		TMap<StratResources, int> resources = UnitActions::GetMoreSpecificFactionResources(faction);
		TMap<WorkerType, int> workers = UnitActions::GetFactionWorkers(faction);

		if (resources[StratResources::Production] >= troopCosts[unit].productionCost && workers[WorkerType::Human] > troopCosts[unit].populationCost)
		{
			canAfford = true;
		}

		resourceCosts.Add(StratResources::Production, troopCosts[unit].productionCost);
		popCosts.Add(WorkerType::Human, troopCosts[unit].populationCost);
	}

	if (canAfford) UnitActions::ConsumeSpentResources(faction, resourceCosts, popCosts, nullptr);

	return canAfford;
}

void AGlobalSpawner::BuildTroop(Factions faction, UnitTypes unit, ABaseHex* hex, AOutpost* outpost)
{
	FTroopStats unitData = troopStats[unit];	

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	UnitActions::AssignFaction(faction, newTroop);
	UnitActions::ApplyDataToUnitStats(newTroop->unitStats, unitData);


	if (outpost && unit == UnitTypes::Settler)
	{
		Cast<ASettler>(newTroop)->popInStorage += troopCosts[unit].populationCost;
		//outpost->popInStorage -= troopCosts[unit].populationCost;
	}
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

