// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/HexManager.h"
#include "GlobalSpawner.h"
#include "HexBiomeMaskComponent.h"
#include "stdlib.h"

AHexManager* AHexManager::instance = nullptr;
FBox AHexManager::mapBounds = FBox(FVector::One() * std::numeric_limits<float>::min(), FVector::One() * std::numeric_limits<float>::max());

// Sets default values
AHexManager::AHexManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	biomeMask = CreateDefaultSubobject<UHexBiomeMaskComponent>(TEXT("Biome Mask"));

	hexInfo.Add(TerrainType::None,
		FHexInfo{
			"", "", nullptr, FRandomInt{0, 0}, FRandomInt{0, 0}, FRandomInt{0, 0}, FRandomInt{0, 0}, 0, 0, 0, 0, false,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TilePlains.TilePlains'")),
			nullptr
		});
	hexInfo.Add(TerrainType::Plains, 
		FHexInfo{ 
			// Name
			TEXT("Moldy Plains"),
			// Description
			TEXT("Flat terrain with no unique benefits."), 
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Plain.Landscape_Icon_Plain'")),
			// Food
			FRandomInt{1, 5},
			// Production
			FRandomInt{1, 5}, 
			// Energy
			FRandomInt{0, 4},
			// Wealth
			FRandomInt{0, 5}, 
			// Defender Bonus
			0, 
			// Move Multi
			1.f, 
			// Attrition Multi
			1.f, 
			// Vision
			0,
			// Can See Into
			true,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TilePlains.TilePlains'")),
			// Topper
			nullptr
		 });
	hexInfo.Add(TerrainType::Forest,
		FHexInfo{
			// Name
			TEXT("Fungal Forest"),
			// Description
			TEXT("Full of tall mushrooms and food."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Forest.Landscape_Icon_Forest'")),
			// Food
			FRandomInt{2, 6},
			// Production
			FRandomInt{0, 3},
			// Energy
			FRandomInt{1, 5},
			// Wealth
			FRandomInt{0, 2},
			// Defender Bonus
			1,
			// Move Multi
			1.15f,
			// Attrition Multi
			1.1f,
			// Vision
			0,
			// Can See Into
			true,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForest.TileForest'")),
			// Topper
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForestModel_TileForest.TileForestModel_TileForest'"))
		});
	hexInfo.Add(TerrainType::Jungle,
		FHexInfo{
			// Name
			TEXT("Oozing Jungle"),
			// Description
			TEXT("Dense, humid, and sticky fungal growths."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Dense_Forest.Landscape_Icon_Dense_Forest'")),
			// Food
			FRandomInt{1, 5},
			// Production
			FRandomInt{0, 4},
			// Energy
			FRandomInt{1, 5},
			// Wealth
			FRandomInt{0, 3},
			// Defender Bonus
			1,
			// Move Multi
			1.3f,
			// Attrition Multi
			1.2f,
			// Vision
			-1,
			// Can See Into
			false,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'")),
			// Topper
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungleModel_TileForestThick.TileJungleModel_TileForestThick'"))
		});
	hexInfo.Add(TerrainType::Hills,
		FHexInfo{
			// Name
			TEXT("Capped Hills"),
			// Description
			TEXT("Compressed mushroom caps create hills."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Hills_TEMP.Landscape_Icon_Hills_TEMP'")),
			// Food
			FRandomInt{1, 3},
			// Production
			FRandomInt{1, 6},
			// Energy
			FRandomInt{1, 3},
			// Wealth
			FRandomInt{0, 3},
			// Defender Bonus
			1,
			// Move Multi
			1.2f,
			// Attrition Multi
			1.1f,
			// Vision
			1,
			// Can See Into
			true,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileHills.TileHills'")),
			// Topper
			nullptr
		});
	hexInfo.Add(TerrainType::Mountains,
		FHexInfo{
			// Name
			TEXT("Stemstack Mountains"),
			// Description
			TEXT("High intensity of mushroom stems in mountain formations make traversal and construction impossible."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Mountain.Landscape_Icon_Mountain'")),
			// Food
			FRandomInt{0, 0},
			// Production
			FRandomInt{0, 0},
			// Energy
			FRandomInt{0, 0},
			// Wealth
			FRandomInt{0, 0},
			// Defender Bonus
			0,
			// Move Multi
			0.f,
			// Attrition Multi
			0.f,
			// Vision
			0,
			// Can See Into
			false,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileMountainModel.TileMountainModel'")),
			// Topper
			nullptr
		});
	hexInfo.Add(TerrainType::SporeField,
		FHexInfo{
			// Name
			TEXT("Toxic Spore Field"),
			// Description
			TEXT("Dangerous, toxic spores provide energy."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Landscape_Icon_Toxic.Landscape_Icon_Toxic'")),
			// Food
			FRandomInt{0, 2},
			// Production
			FRandomInt{0, 2},
			// Energy
			FRandomInt{2, 6},
			// Wealth
			FRandomInt{0, 2},
			// Defender Bonus
			0,
			// Move Multi
			1.2f,
			// Attrition Multi
			1.0f,
			// Vision
			0,
			// Can See Into
			true,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileSporefieldModel.TileSporefieldModel'")),
			// Topper
			nullptr
		});
	hexInfo.Add(TerrainType::Ship,
		FHexInfo{
			// Name
			TEXT("Capitol Hub"),
			// Description
			TEXT("The crash site of the ship, now the base of operations."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Capital_Hub/Hills_Capital_Hub.Hills_Capital_Hub'")),
			// Food
			FRandomInt{2, 2},
			// Production
			FRandomInt{3, 3},
			// Energy
			FRandomInt{4, 4},
			// Wealth
			FRandomInt{0, 0},
			// Defender Bonus
			2,
			// Move Multi
			1.0f,
			// Attrition Multi
			1.0f,
			// Vision
			0,
			// Can See Into
			true,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileForest.TileForest'")),
			// Topper
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/BuildingCapitolShip2.BuildingCapitolShip2'"))
		});
	hexInfo.Add(TerrainType::AlienCity,
		FHexInfo{
			// Name
			TEXT("Normal Klequeen City"),
			// Description
			TEXT("Alien city, some stupid piece of shit you shouldn't use."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Alien_city/Siatus_City.Siatus_City'")),
			// Food
			FRandomInt{2, 6},
			// Production
			FRandomInt{1, 5},
			// Energy
			FRandomInt{1, 4},
			// Wealth
			FRandomInt{1, 3},
			// Defender Bonus
			2,
			// Move Multi
			1.0f,
			// Attrition Multi
			1.0f,
			// Vision
			0,
			// Can See Into
			true,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'")),
			// Topper
			nullptr
		});
	hexInfo.Add(TerrainType::TheRock,
		FHexInfo{
			// Name
			TEXT("The Rock City"),
			// Description
			TEXT("The Rock, contains DST fuel."),
			// Icon
			LoadObject<UTexture2D>(nullptr, TEXT("Texture2D '/Game/Art_Assets/Landscape_Biomes/Alien_city/Rock_City_Octagon.Rock_City_Octagon'")),
			// Food
			FRandomInt{3, 7},
			// Production
			FRandomInt{3, 7},
			// Energy
			FRandomInt{3, 7},
			// Wealth
			FRandomInt{3, 7},
			// Defender Bonus
			3,
			// Move Multi
			1.0f,
			// Attrition Multi
			1.0f,
			// Vision
			1,
			// Can See Into
			true,
			// Starting Seed Size
			5,
			// Model
			LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Vertical_Slice_Assets/TileJungle.TileJungle'")),
			// Topper
			nullptr
		});
}

// Called when the game starts or when spawned
void AHexManager::BeginPlay()
{
	Super::BeginPlay();
	instance = this;

	CreateHexGrid();
	CalculateMapBoundingBox();
}

void AHexManager::CreateHexGrid()
{
	TArray<TArray<HexSeed>> seedGrid;
	for (int x = 0; x < gridColumns; x++)
	{
		TArray<HexSeed> column;
		TArray<ABaseHex*> hexColumn;

		for (int y = 0; y < gridRows; y++)
		{
			HexSeed seed = HexSeed{TerrainType::None, FVector2D(x, y), 0};
			column.Add(seed);
			hexColumn.Add(nullptr);
		}

		seedGrid.Add(column);
		hexArray.Add(hexColumn);
	}

	HexSeed& capitolHex = seedGrid
		[FMath::RandRange((int)capitolCoordsMin.X, (int)capitolCoordsMax.X)]
		[FMath::RandRange((int)capitolCoordsMin.Y, (int)capitolCoordsMax.Y)];
	capitolHex.terrain = TerrainType::Ship;
	SetTilesInRadius(capitolHex, seedGrid, AGlobalSpawner::spawnerObject->buildingCosts[SpawnableBuildings::Capitol].size, terrainSurroundingImportantTiles);

	HexSeed& rockHex = seedGrid
		[FMath::RandRange((int)rockCityCoordsMin.X, (int)rockCityCoordsMax.X)]
		[FMath::RandRange((int)rockCityCoordsMin.Y, (int)rockCityCoordsMax.Y)];
	rockHex.terrain = TerrainType::TheRock;
	SetTilesInRadius(rockHex, seedGrid, AGlobalSpawner::spawnerObject->buildingCosts[SpawnableBuildings::RockCity].size, terrainSurroundingImportantTiles);

	for (int x = 0; x < gridColumns; x++)
	{
		for (int y = 0; y < gridRows; y++)
		{
			HexSeed& currentSeed = seedGrid[x][y];

			if (currentSeed.terrain == TerrainType::None)
			{
				CalculateHexSeed(currentSeed, seedGrid);
				if (currentSeed.terrain == TerrainType::None) continue;
			}

			float oddHex = (y % 2 > 0) ? 1 : 0;

			FVector spawnPos = gridOrigin + FVector(-(stepDistanceY * y), -(stepDistanceX * x + (oddHex * stepDistanceX / 2)), 0.f);

			ABaseHex* newHex = GetWorld()->SpawnActor<ABaseHex>(hexActor, spawnPos, FRotator::ZeroRotator);
			newHex->SetHexCoordinates(x, y);
			newHex->PrintCoordinates(x, y);

			newHex->SetHexTerrain(currentSeed.terrain);

			hexArray[x][y] = newHex;
		}
	}

	biomeMask->BuildGrid(hexArray);

	for (int x = 0; x < gridColumns; x++)
	{
		for (int y = 0; y < gridRows; y++)
		{
			ABaseHex* hex = hexArray[x][y];
			if (!hex) continue;
			biomeMask->SetTileOwner(hex->GetActorLocation(), (uint8)hex->GetHexOwner());
		}
	}

	AGlobalSpawner::spawnerObject->SpawnBuildingFree(EFactions::Alien1, SpawnableBuildings::RockCity, hexArray[rockHex.coords.X][rockHex.coords.Y], true);
}

void AHexManager::CalculateHexSeed(HexSeed& seed, const TArray<TArray<HexSeed>>& seedGrid)
{
	//Declare variable to use as return value
	TerrainType terrainToSet = TerrainType::None;
	int maxX = AHexManager::GetGridColumnMax() - 1, maxY = AHexManager::GetGridRowMax() - 1;

	// If the hex exists in the outer border of the grid, guarantee a None terrain
	if (createNoneTiles)
	{
		if (seed.coords.X <= marginForGuaranteedNoneTerrain || seed.coords.X >= maxX - marginForGuaranteedNoneTerrain ||
			seed.coords.Y <= marginForGuaranteedNoneTerrain || seed.coords.Y >= maxY - marginForGuaranteedNoneTerrain)
		{
			seed.terrain = TerrainType::None;
			seed.currentSeed = hexInfo[TerrainType::None].startingSeedSize;
			return;
		}
	}

	//Get surrounding hexes
	TSet<FVector2D> surroundingSeeds = GetCoordinatesInRadius(seed.coords, EBuildingSize::SevenTiles);

	bool randTerrain = false;
	int highestSeed = 0;

	//Compare seed values for highest value, set return variable to hex with that value
	for (const FVector2D& coord : surroundingSeeds)
	{
		const HexSeed& hexSeed = seedGrid[coord.X][coord.Y];
		if (hexSeed.currentSeed > highestSeed)
		{
			highestSeed = hexSeed.currentSeed;
			terrainToSet = hexSeed.terrain;
		}
	}

	//If the highest seed found is 0, either no seed was previously set, or the end of an existing seed was reached
	//Regardless, this is a prompt to set this hex to a new random terrain
	if (highestSeed <= 0)
	{
		randTerrain = true;
	}
	else
	{
		//If a seed was found, get a random number from 1-10 and add the found seed
		//This makes growth of the current seed stronger in its beginnings
		int rand = FMath::RandRange(1, 10) + seed.currentSeed;

		//If the random number is greater than the minimum number to maintain, use the highest seed index terrain
		//Set this hex's seed index to the highest found minus 1 to reduce chances of continued growth in the next hex
		if (rand > hexRandToMaintain)
		{
			seed.terrain = terrainToSet;
			seed.currentSeed = highestSeed - 1;
		}
		else
		{
			//Otherwise, prompt the beginning of a new random terrain seed
			randTerrain = true;
		}
	}

	//If randomization was prompted, pick a random terrain type and set the seed index to the maximum
	if (randTerrain)
	{
		terrainToSet = TerrainType(FMath::RandRange(createNoneTiles ? 0 : 1, 6));
		seed.terrain = terrainToSet;
		seed.currentSeed = hexInfo[terrainToSet].startingSeedSize;
	}
}

void AHexManager::SetTilesInRadius(const HexSeed& seed, TArray<TArray<HexSeed>>& seedGrid, EBuildingSize size, TerrainType terrain)
{
	TSet<FVector2D> surroundingTiles = GetCoordinatesInRadius(seed.coords, size);
	for (const FVector2D& tile : surroundingTiles)
	{
		seedGrid[tile.X][tile.Y].terrain = terrain;
		seedGrid[tile.X][tile.Y].currentSeed = hexInfo[terrain].startingSeedSize;
	}
}

TSet<FVector2D> AHexManager::GetCoordinatesInRadius(FVector2D coordinate, EBuildingSize size) const
{
	int32 maxX = AHexManager::GetGridColumnMax() - 1, maxY = AHexManager::GetGridRowMax() - 1;
	int32 radius = BuildingSizeToRadius(size);

	TSet<FVector2D> tiles;
	TQueue<FVector2D> found;

	int dirOdd[6][2] = { {1,0}, {-1,0}, {0,-1}, {1,1}, {0, 1}, {1,-1} };
	int dirEven[6][2] = { {1,0}, {-1,0}, {-1,-1}, {0,1}, {-1,1}, {0,-1} };

	auto GetAdjacentTiles = [&](const FVector2D& centerTile)
		{
			int32 coordX = FMath::RoundToInt32(centerTile.X);
			int32 coordY = FMath::RoundToInt32(centerTile.Y);

			int (*dir)[2] = coordY % 2 == 0 ? dirEven : dirOdd;

			for (int i = 0; i < 6; i++)
			{
				int x = coordX + dir[i][0];
				int y = coordY + dir[i][1];

				if (x > maxX || x < 0 || y > maxY || y < 0) continue;

				FVector2D tile = FVector2D(x, y);
				if (!tiles.Contains(tile))
				{
					found.Enqueue(tile);
					tiles.Add(tile);
				}
			}
		};

	found.Enqueue(coordinate);

	for (int i = 0; i < radius; i++)
	{
		TQueue<FVector2D> layer;
		FVector2D currentVal = FVector2D();

		while (found.Dequeue(currentVal))
		{
			layer.Enqueue(currentVal);
		}

		while (layer.Dequeue(currentVal))
		{
			GetAdjacentTiles(currentVal);
		}
	}

	if (tiles.Contains(coordinate))
		tiles.Remove(coordinate);

	return tiles;
}

void AHexManager::CalculateMapBoundingBox()
{
	float xMax = std::numeric_limits<float>::min();
	float xMin = std::numeric_limits<float>::max();
	float yMax = std::numeric_limits<float>::min();
	float yMin = std::numeric_limits<float>::max();

	for (const TArray<ABaseHex*>& array : hexArray)
	{
		for (ABaseHex* hex : array)
		{
			if (!hex)
			{
				continue;
			}

			FBox hexBoundingBox = hex->GetComponentsBoundingBox();
			if (xMax < hexBoundingBox.Max.X)
			{
				xMax = hexBoundingBox.Max.X;
			}

			if (xMin > hexBoundingBox.Min.X)
			{
				xMin = hexBoundingBox.Min.X;
			}

			if (yMax < hexBoundingBox.Max.Y)
			{
				yMax = hexBoundingBox.Max.Y;
			}

			if (yMin > hexBoundingBox.Min.Y)
			{
				yMin = hexBoundingBox.Min.Y;
			}
		}
	}

	FVector maxVector = FVector(xMax, yMax, gridOrigin.Z);
	FVector minVector = FVector(xMin, yMin, gridOrigin.Z);

	FBox mapBoundsBox = FBox(minVector, maxVector);

	mapBounds = mapBoundsBox;
}

FHexInstance AHexManager::CreateHexInstance(TerrainType terrain)
{
	return FHexInstance(instance->hexInfo[terrain]);
}

void AHexManager::CreateHexModel(TerrainType terrainType, ABaseHex* hex)
{
	if (!instance || !instance->hexInfo.Contains(terrainType)) return;

	UStaticMesh* meshAsset = instance->hexInfo[terrainType].model;
	UStaticMesh* extraAsset = instance->hexInfo[terrainType].topper;

	if (meshAsset)
	{
		hex->hexMesh->SetStaticMesh(meshAsset);
		hex->hexMesh->EmptyOverrideMaterials();
	}

	if (extraAsset)
	{
		hex->hexMeshAttachment->SetStaticMesh(extraAsset);
		hex->hexMeshAttachment->EmptyOverrideMaterials();

		if (terrainType != TerrainType::Ship)
		{
			float randomRot = FMath::RandRange(0.f, 360.f);
			hex->hexMeshAttachment->SetRelativeRotation(FRotator(0, randomRot, 0));
		}
	}

	hex->hexMeshAttachment->SetVisibility(extraAsset != nullptr);
	hex->SetAttachmentCanBeVisible(extraAsset != nullptr);

	EFactions owner = hex->GetHexOwner();

	hex->visibility->ResetComponent();
	hex->visibility->SetupComponent(owner, hex->hexMesh);
	hex->visibility->SetupComponent(owner, hex->hexMeshAttachment);


	// Alex Code - Write the biome and hand the material its parameters
	if (instance->biomeMask)
	{
		instance->biomeMask->SetTileBiome(hex->GetActorLocation(), UHexBiomeMaskComponent::TerrainToBiomeId(hex->GetHexTerrain()));
		instance->biomeMask->ApplyToMaterial(Cast<UMaterialInstanceDynamic>(hex->hexMesh->GetMaterial(0)));
	}
}

ABaseHex* AHexManager::GetHexFromCoordinates(int x, int y)
{
	if (x < 0 || y < 0) return nullptr;

	if (instance->hexArray.Num() - 1 > x)
	{
		if (instance->hexArray[x].Num() - 1 > y)
		{
			return instance->hexArray[x][y];
		}
	}

	return nullptr;
}

int AHexManager::GetGridRowMax()
{
	if (!instance) return 0;
	return instance->gridRows;
}

int AHexManager::GetGridColumnMax()
{
	if (!instance) return 0;
	return instance->gridColumns;
}

void AHexManager::UpdateHexOwnerVisual(ABaseHex* hex)
{
	if (!instance || !instance->biomeMask) return;

	instance->biomeMask->SetTileOwner(hex->GetActorLocation(), (uint8)hex->GetHexOwner());
}

void AHexManager::RevealHex(ABaseHex* hex)
{
	if (!instance || !instance->biomeMask) return;

	instance->biomeMask->RevealTile(hex->GetActorLocation());
}

FBox AHexManager::GetMapBounds()
{
	return mapBounds;
}
