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

	buildingCosts.Add(SpawnableBuildings::MiningStation, FBuildingCost{ 200, 30, 180, FText::FromString("Mining Station"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Production_Station.Production_Station'")) });
	buildingCosts.Add(SpawnableBuildings::Farmland, FBuildingCost{ 100, 30, 150, FText::FromString("Farmland"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Food_Farming_Station.Food_Farming_Station'"))});
	buildingCosts.Add(SpawnableBuildings::PowerPlant, FBuildingCost{ 200, 30, 200, FText::FromString("Power Plant"),
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/StationIcons/Energy_Station.Energy_Station'")) });
	buildingCosts.Add(SpawnableBuildings::Outpost, FBuildingCost{ 0, 0, 300, FText::FromString("Outpost"),
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

	troopCosts.Add(SpawnableUnits::Infantry, FTroopCost{ 100, 48, 0, FText::FromString("Infantry"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/ArmyUnitIcons/Infantry.Infantry'")) });
	troopCosts.Add(SpawnableUnits::Cavalry, FTroopCost{ 100, 60, 0, FText::FromString("Cavalry"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/ArmyUnitIcons/Cavalry.Cavalry'")) });
	troopCosts.Add(SpawnableUnits::Ranged, FTroopCost{ 100, 48, 0, FText::FromString("Ranged"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/ArmyUnitIcons/Ranged.Ranged'")) });
	troopCosts.Add(SpawnableUnits::Shielder, FTroopCost{ 100, 60, 0, FText::FromString("Shielder"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/ArmyUnitIcons/Shielder.Shielder'")) });
	troopCosts.Add(SpawnableUnits::Scout, FTroopCost{ 20, 24, 0, FText::FromString("Scout"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/ArmyUnitIcons/Scout.Scout'")) });
	troopCosts.Add(SpawnableUnits::Settler, FTroopCost{ 400, 60, 50, FText::FromString("Settler"), 
		LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Icons/ArmyUnitIcons/Settler.Settler'")) });

	troopStats.Add(SpawnableUnits::Infantry, FTroopStats{ FText::FromString("Infantry"), FText::FromString("Goofy ahh"), UnitTypes::Infantry, 100, 60, 6, 10, 1, 100, 1, 1 });
	troopStats.Add(SpawnableUnits::Cavalry, FTroopStats{ FText::FromString("Cavalry"), FText::FromString("Goofy ahh"), UnitTypes::Cavalry, 120, 45, 8, 5, 1, 100, 1, 1 });
	troopStats.Add(SpawnableUnits::Ranged, FTroopStats{ FText::FromString("Ranged"), FText::FromString("Goofy ahh"), UnitTypes::Ranged, 80, 80, 4, 20, 2, 100, 1, 1 });
	troopStats.Add(SpawnableUnits::Shielder, FTroopStats{ FText::FromString("Shielder"), FText::FromString("Goofy ahh"), UnitTypes::Shielder, 150, 75, 3, 15, 1, 100, 1, 1 });
	troopStats.Add(SpawnableUnits::Scout, FTroopStats{ FText::FromString("Scout"), FText::FromString("Goofy ahh"), UnitTypes::Scout, 50, 30, 2, 1, 3, 100, 1, 1 });
	troopStats.Add(SpawnableUnits::Settler, FTroopStats{ FText::FromString("Settler"), FText::FromString("Goofy ahh"), UnitTypes::Settler, 75, 60, 0, 0, 1, 100, 1, 1 });
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
	float offsetSide = 0.0f;
	float offsetUp = 0.0f;
	FVector origin = GetActorLocation();
	double sqroot;
	int roundedSQRoot;
	AActor* newHex;

	TArray<TArray<AActor*>> arrayOfHexColumns;

	switch (shape)
	{
	case ShapesOfMap::None:
		break;
	case ShapesOfMap::Square:
		sqroot = FMath::Sqrt(static_cast<double>(numHexs));
		roundedSQRoot = sqroot;


		for (int i = 0; i < roundedSQRoot; i++)
		{
			for (int j = 0; j < roundedSQRoot; j++)
			{
				newHex = GetWorld()->SpawnActor<AActor>(hexActor, origin, FRotator(0,0,0));
				
				
				arrayOfHexColumns[i][j] = newHex;
			}
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

	//newTroop->InputUnitStats(data);
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

bool AGlobalSpawner::PurchaseTroop(Factions faction, SpawnableUnits unit, AOutpost* outpost)
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

void AGlobalSpawner::BuildTroop(Factions faction, SpawnableUnits unit, ABaseHex* hex, AOutpost* outpost)
{
	FTroopStats unitData = troopStats[unit];	

	ATroop* newTroop = GetWorld()->SpawnActor<ATroop>(troopPrefab, hex->troopAnchor->GetComponentLocation(), FRotator(0, 0, 0));
	UnitActions::AssignFaction(faction, newTroop);
	UnitActions::ApplyDataToUnitStats(newTroop->unitStats, unitData);


	if (outpost && unit == SpawnableUnits::Settler)
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

