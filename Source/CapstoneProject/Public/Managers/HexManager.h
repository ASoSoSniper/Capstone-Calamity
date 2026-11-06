// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Types/HexInfo.h"
#include "BaseHex.h"
#include "TerrainEnum.h"
#include "HexManager.generated.h"

class UHexBiomeMaskComponent;

UCLASS(HideCategories = (Rendering, Replication, Input, Actor, LOD, HLOD, Collision, Physics, Cooking, Events, WorldPartition, Events, DataLayers, Tags, ComponentReplication, Activation, Variable, AssetUserData))
class CAPSTONEPROJECT_API AHexManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexManager();

	static FHexInstance CreateHexInstance(TerrainType terrain);
	static void CreateHexModel(TerrainType terrainType, ABaseHex* hex);
	static ABaseHex* GetHexFromCoordinates(int x, int y);
	static int GetGridRowMax();
	static int GetGridColumnMax();
	static void UpdateHexOwnerVisual(ABaseHex* hex);
	static void RevealHex(ABaseHex* hex);
	static TArray<FVector> GetWorldCornerCoordinates();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	struct HexSeed
	{
		TerrainType terrain;
		FVector2D coords;
		int currentSeed;
	};

	void CreateHexGrid();
	void CalculateHexSeed(HexSeed& seed, const TArray<TArray<HexSeed>>& seedGrid);
	void SetTilesInRadius(const HexSeed& seed, TArray<TArray<HexSeed>>& seedGrid, EBuildingSize size, TerrainType terrain);
	TSet<FVector2D> GetCoordinatesInRadius(FVector2D coordinate, EBuildingSize size) const;

	static AHexManager* instance;
	TArray<TArray<ABaseHex*>> hexArray;
	TArray<FVector> worldCornerCoordinates;

	UPROPERTY(VisibleAnywhere, Category = "World Generation") UHexBiomeMaskComponent* biomeMask;
	UPROPERTY(EditAnywhere, Category = "Identity") TMap<TerrainType, FHexInfo> hexInfo;
	UPROPERTY(EditAnywhere, Category = "Identity") TSubclassOf<class ABaseHex> hexActor;

	UPROPERTY(EditAnywhere, Category = "World Generation") int pointOfInterestCount = 15;
	UPROPERTY(EditAnywhere, Category = "World Generation") TArray<FPointOfInterest> pointsOfInterest;
	UPROPERTY(EditAnywhere, Category = "World Generation") bool createNoneTiles = true;
	UPROPERTY(EditAnywhere, Category = "World Generation") FVector2D capitolCoordsMin = { 4, 3 };
	UPROPERTY(EditAnywhere, Category = "World Generation") FVector2D capitolCoordsMax = { 4, 3 };
	UPROPERTY(EditAnywhere, Category = "World Generation") FVector2D rockCityCoordsMin = { 12, 12 };
	UPROPERTY(EditAnywhere, Category = "World Generation") FVector2D rockCityCoordsMax = { 17, 17 };
	UPROPERTY(EditAnywhere, Category = "World Generation") TerrainType terrainSurroundingImportantTiles = TerrainType::Plains;

	UPROPERTY(EditAnywhere, Category = "Hex Grid") int gridRows = 35;
	UPROPERTY(EditAnywhere, Category = "Hex Grid") int gridColumns = 35;
	UPROPERTY(EditAnywhere, Category = "Hex Grid") FVector gridOrigin = FVector(-250.f, 400.f, -1.f);
	UPROPERTY(EditAnywhere, Category = "Hex Grid") float stepDistanceX = 73.5f;
	UPROPERTY(EditAnywhere, Category = "Hex Grid") float stepDistanceY = -63.f;
	UPROPERTY(EditAnywhere, Category = "Hex Grid") int hexRandToMaintain = 5;
	UPROPERTY(EditAnywhere, Category = "Hex Grid") int marginForGuaranteedNoneTerrain = 2;
};
