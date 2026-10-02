// Fill out your copyright notice in the Description page of Project Settings.

#include "HexBiomeMaskComponent.h"
#include "BaseHex.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"

UHexBiomeMaskComponent::UHexBiomeMaskComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHexBiomeMaskComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Many tiles can change in one frame (e.g. map generation), so upload once at most per frame
	if (pixelsDirty)
	{
		UploadPixels();
		pixelsDirty = false;
	}
}

uint8 UHexBiomeMaskComponent::TerrainToBiomeId(TerrainType terrain)
{
	switch (terrain)
	{
	case TerrainType::Hills:
		return 1;
	case TerrainType::SporeField:
		return 2;
	case TerrainType::Forest:
	case TerrainType::Ship:
		return 3;
	case TerrainType::Jungle:
	case TerrainType::AlienCity:
	case TerrainType::TheRock:
	case TerrainType::Border:
		return 4;
	case TerrainType::Mountains:
		return 5;
	default:
		return 0; //Plains and None
	}
}

//Must match the HexBiomeMask Custom node math exactly
FIntPoint UHexBiomeMaskComponent::WorldToCell(const FVector& worldLocation) const
{
	const float sqrt3 = FMath::Sqrt(3.f);
	const float r = outerRadius;

	FVector2D p = FVector2D(worldLocation.X, worldLocation.Y) - gridOrigin;
	p = FVector2D(p.Y, p.X);
	p = FVector2D(p.X / cellStep.X * sqrt3 * r, p.Y / cellStep.Y * 1.5f * r);

	const float qf = (sqrt3 / 3.f * p.X - 1.f / 3.f * p.Y) / r;
	const float rf = (2.f / 3.f * p.Y) / r;
	const float sf = -qf - rf;

	float cq = FMath::RoundToFloat(qf);
	float cr = FMath::RoundToFloat(rf);
	const float cs = FMath::RoundToFloat(sf);

	const float dq = FMath::Abs(cq - qf);
	const float dr = FMath::Abs(cr - rf);
	const float ds = FMath::Abs(cs - sf);

	if (dq > dr && dq > ds) cq = -cr - cs;
	else if (dr > ds) cr = -cq - cs;

	const int32 q = (int32)cq;
	const int32 row = (int32)cr;
	return FIntPoint(q + FMath::FloorToInt(row * 0.5f), row);
}

void UHexBiomeMaskComponent::BuildGrid(const TArray<TArray<ABaseHex*>>& hexGrid)
{
	TArray<FIntPoint> cells;
	FIntPoint minCell(MAX_int32, MAX_int32);
	FIntPoint maxCell(MIN_int32, MIN_int32);

	for (const TArray<ABaseHex*>& column : hexGrid)
	{
		for (ABaseHex* hex : column)
		{
			if (!hex) continue;

			const FIntPoint cell = WorldToCell(hex->GetActorLocation());
			cells.Add(cell);

			minCell.X = FMath::Min(minCell.X, cell.X);
			minCell.Y = FMath::Min(minCell.Y, cell.Y);
			maxCell.X = FMath::Max(maxCell.X, cell.X);
			maxCell.Y = FMath::Max(maxCell.Y, cell.Y);
		}
	}

	if (cells.IsEmpty()) return;

	cellOffset = FIntPoint(-minCell.X, -minCell.Y);
	texSize = maxCell - minCell + FIntPoint(1, 1);

	//Verification: two tiles on one pixel means the C++/shader math doesn't match the spawned grid
	TSet<FIntPoint> usedCells;
	int32 duplicates = 0;
	for (const FIntPoint& cell : cells)
	{
		bool alreadyUsed = false;
		usedCells.Add(cell, &alreadyUsed);
		if (alreadyUsed) duplicates++;
	}

	UE_LOG(LogTemp, Log, TEXT("HexBiomeMask: %d tiles, texture %dx%d, CellOffset (%d, %d)"),
		cells.Num(), texSize.X, texSize.Y, cellOffset.X, cellOffset.Y);
	if (duplicates > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("HexBiomeMask: %d tiles share a pixel with another tile. Grid math does not match the spawner!"), duplicates);
	}

	//R = 255 means "no tile"
	pixels.Init(FColor(255, 0, 0, 255), texSize.X * texSize.Y);

	cellTexture = UTexture2D::CreateTransient(texSize.X, texSize.Y, PF_B8G8R8A8);
	cellTexture->Filter = TF_Nearest;
	cellTexture->SRGB = false;
	cellTexture->AddressX = TA_Clamp;
	cellTexture->AddressY = TA_Clamp;
	cellTexture->NeverStream = true;
	cellTexture->UpdateResource();

	pixelsDirty = true;
}

void UHexBiomeMaskComponent::SetTileBiome(const FVector& worldLocation, uint8 biomeId)
{
	if (!cellTexture) return;

	const FIntPoint p = WorldToCell(worldLocation) + cellOffset;
	if (p.X < 0 || p.Y < 0 || p.X >= texSize.X || p.Y >= texSize.Y) return;

	pixels[p.Y * texSize.X + p.X].R = biomeId;
	pixelsDirty = true;
}

void UHexBiomeMaskComponent::ApplyToMaterial(UMaterialInstanceDynamic* material) const
{
	if (!material || !cellTexture) return;

	material->SetTextureParameterValue(TEXT("CellData"), cellTexture);
	material->SetVectorParameterValue(TEXT("GridSize"), FLinearColor(texSize.X, texSize.Y, 0.f, 0.f));
	material->SetVectorParameterValue(TEXT("GridOrigin"), FLinearColor(gridOrigin.X, gridOrigin.Y, 0.f, 0.f));
	material->SetVectorParameterValue(TEXT("CellStep"), FLinearColor(cellStep.X, cellStep.Y, 0.f, 0.f));
	material->SetVectorParameterValue(TEXT("CellOffset"), FLinearColor(cellOffset.X, cellOffset.Y, 0.f, 0.f));
	material->SetScalarParameterValue(TEXT("OuterRadius"), outerRadius);
}

void UHexBiomeMaskComponent::UploadPixels()
{
	if (!cellTexture || pixels.IsEmpty()) return;

	//The GPU copy happens later on the render thread, so hand it its own copy of the pixels
	const int32 count = pixels.Num();
	FColor* copy = new FColor[count];
	FMemory::Memcpy(copy, pixels.GetData(), count * sizeof(FColor));

	FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(0, 0, 0, 0, texSize.X, texSize.Y);
	cellTexture->UpdateTextureRegions(0, 1, region, texSize.X * sizeof(FColor), sizeof(FColor), (uint8*)copy,
		[](uint8* data, const FUpdateTextureRegion2D* regions)
		{
			delete[](FColor*)data;
			delete regions;
		});
}