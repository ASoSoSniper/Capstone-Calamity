// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TerrainEnum.h"
#include "HexBiomeMaskComponent.generated.h"

class ABaseHex;
class UTexture2D;
class UMaterialInstanceDynamic;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CAPSTONEPROJECT_API UHexBiomeMaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHexBiomeMaskComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Call once after every tile is spawned. Sizes the texture and fills it with 255 (no tile)
	void BuildGrid(const TArray<TArray<ABaseHex*>>& hexGrid);

	//Write one tile's biome into the texture. Uploaded to the GPU on the next tick
	void SetTileBiome(const FVector& worldLocation, uint8 biomeId);

	//Give one tile's material instance the biome texture and grid parameters
	void ApplyToMaterial(UMaterialInstanceDynamic* material) const;

	static uint8 TerrainToBiomeId(TerrainType terrain);

	UPROPERTY(EditAnywhere, Category = "Hex Grid") FVector2D gridOrigin = FVector2D(-250.f, 400.f);
	UPROPERTY(EditAnywhere, Category = "Hex Grid") FVector2D cellStep = FVector2D(73.5f, 63.f);
	UPROPERTY(EditAnywhere, Category = "Hex Grid") float outerRadius = 42.4f;

private:
	FIntPoint WorldToCell(const FVector& worldLocation) const;
	void UploadPixels();

	UPROPERTY() UTexture2D* cellTexture = nullptr;
	TArray<FColor> pixels;
	FIntPoint texSize = FIntPoint::ZeroValue;
	FIntPoint cellOffset = FIntPoint::ZeroValue;
	bool pixelsDirty = false;
};