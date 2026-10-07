// Fill out your copyright notice in the Description page of Project Settings.

#include "HexBiomeMaskComponent.h"
#include "BaseHex.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GlobalSpawner.h" // Territory (Option B)
#include "Kismet/GameplayStatics.h"       // Fog clouds
#include "Components/StaticMeshComponent.h" // Fog clouds

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
		UploadPixels(cellTexture, pixels);
		pixelsDirty = false;
	}

	// Fog: recalculate frontier distances once per frame, however many tiles were revealed
	if (frontierDirty)
	{
		UpdateFrontier();
		frontierDirty = false;
		fogDirty = true;
	}

	// Fog: ease each frontier distance toward its target so the fog recedes smoothly instead of popping
	if (frontierAnimating)
	{
		frontierAnimating = false;
		for (int32 i = 0; i < frontierSmooth.Num(); i++)
		{
			if (frontierSmooth[i] == frontierTarget[i]) continue;

			frontierSmooth[i] = FMath::FInterpConstantTo(frontierSmooth[i], frontierTarget[i], DeltaTime, fogRecedeSpeed);
			fogPixels[i].G = (uint8)FMath::RoundToInt(frontierSmooth[i] * 16.f);

			if (frontierSmooth[i] != frontierTarget[i]) frontierAnimating = true;
		}
		fogDirty = true;
	}

	// Fog: animate tiles that are being revealed
	if (revealing.Num() > 0)
	{
		const float step = DeltaTime / FMath::Max(revealDuration, 0.01f);
		for (int32 i = revealing.Num() - 1; i >= 0; i--)
		{
			const int32 index = revealing[i];
			revealAmount[index] = FMath::Min(revealAmount[index] + step, 1.f);
			fogPixels[index].R = (uint8)FMath::RoundToInt(revealAmount[index] * 255.f);
			if (revealAmount[index] >= 1.f) revealing.RemoveAt(i);
		}
		fogDirty = true;
	}

	// Fog
	if (fogDirty)
	{
		UploadPixels(fogTexture, fogPixels);
		fogDirty = false;
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

	// Fog: grow both textures by an off-map ring on every side
	cellOffset += FIntPoint(mapPadding, mapPadding);
	texSize += FIntPoint(mapPadding * 2, mapPadding * 2);

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

	// Fog: second texture, same size. R = reveal (0 hidden, 255 revealed), G = tiles to nearest revealed tile
	const int32 count = texSize.X * texSize.Y;
	
	// Fog: B = 255 marks off-map (ocean) cells, B = 0 marks real tiles
	// Fog: G = animated distance x16, A = target distance x16 (so frontierCap must stay 15 or lower)
	fogPixels.Init(FColor(0, (uint8)(frontierCap * 16), 255, (uint8)(frontierCap * 16)), count);
	frontierSmooth.Init((float)frontierCap, count);
	frontierTarget.Init((float)frontierCap, count);
	frontierAnimating = false;
	frontierInitialized = false;	for (const FIntPoint& cell : cells)
	{
		const FIntPoint p = cell + cellOffset;
		fogPixels[p.Y * texSize.X + p.X].B = 0;
	}

	revealAmount.Init(0.f, count);
	revealed.Init(false, count);
	revealing.Empty();

	fogTexture = UTexture2D::CreateTransient(texSize.X, texSize.Y, PF_B8G8R8A8);
	fogTexture->Filter = TF_Nearest;
	fogTexture->SRGB = false;
	fogTexture->AddressX = TA_Clamp;
	fogTexture->AddressY = TA_Clamp;
	fogTexture->NeverStream = true;
	fogTexture->UpdateResource();
	fogDirty = true;

	// Fog clouds: the fog texture exists now, so hook up the cloud layers
	SetupFogClouds();
}

void UHexBiomeMaskComponent::SetTileBiome(const FVector& worldLocation, uint8 biomeId)
{
	if (!cellTexture) return;

	const FIntPoint p = WorldToCell(worldLocation) + cellOffset;
	if (p.X < 0 || p.Y < 0 || p.X >= texSize.X || p.Y >= texSize.Y) return;

	pixels[p.Y * texSize.X + p.X].R = biomeId;
	pixelsDirty = true;
}

// Territory
void UHexBiomeMaskComponent::SetTileOwner(const FVector& worldLocation, uint8 factionId)
{
	if (!cellTexture) return;

	const FIntPoint p = WorldToCell(worldLocation) + cellOffset;
	if (p.X < 0 || p.Y < 0 || p.X >= texSize.X || p.Y >= texSize.Y) return;

	pixels[p.Y * texSize.X + p.X].B = factionId;
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
	material->SetTextureParameterValue(TEXT("FogData"), fogTexture); // Fog
	material->SetScalarParameterValue(TEXT("FrontierCap"), frontierCap); // Fog



	// Territory (Option B): send each faction's tileColor to the material as FactionColor1 ... FactionColor7
	if (AGlobalSpawner* spawner = Cast<AGlobalSpawner>(GetOwner()))
	{
		for (uint8 i = 1; i < 8; i++)
		{
			if (FFactionDisplay* display = spawner->GetFactionDisplayPreset((EFactions)i))
			{
				material->SetVectorParameterValue(FName(*FString::Printf(TEXT("FactionColor%d"), i)), display->tileColor);
			}
		}
	}
}

void UHexBiomeMaskComponent::UploadPixels(UTexture2D* texture, const TArray<FColor>& source)
{
	if (!texture || source.IsEmpty()) return;

	//The GPU copy happens later on the render thread, so hand it its own copy of the pixels
	const int32 count = source.Num();
	FColor* copy = new FColor[count];
	FMemory::Memcpy(copy, source.GetData(), count * sizeof(FColor));

	FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(0, 0, 0, 0, texSize.X, texSize.Y);
	texture->UpdateTextureRegions(0, 1, region, texSize.X * sizeof(FColor), sizeof(FColor), (uint8*)copy,
		[](uint8* data, const FUpdateTextureRegion2D* regions)
		{
			delete[](FColor*)data;
			delete regions;
		});
}


// Fog
void UHexBiomeMaskComponent::RevealTile(const FVector& worldLocation)
{
	if (!fogTexture) return;

	const FIntPoint p = WorldToCell(worldLocation) + cellOffset;
	if (p.X < 0 || p.Y < 0 || p.X >= texSize.X || p.Y >= texSize.Y) return;

	const int32 index = p.Y * texSize.X + p.X;
	if (revealed[index]) return; // reveal is one-way, so each tile only ever does this once

	revealed[index] = true;
	revealing.Add(index);
	frontierDirty = true;
}

// Fog: which pixel is the neighbouring hex in one of the 6 directions
FIntPoint UHexBiomeMaskComponent::TexelNeighbour(const FIntPoint& texel, int32 direction) const
{
	static const FIntPoint dirs[6] = { FIntPoint(1, 0), FIntPoint(1, -1), FIntPoint(0, -1),
									   FIntPoint(-1, 0), FIntPoint(-1, 1), FIntPoint(0, 1) };

	//Pixel -> hex coordinate (undo CellOffset and the odd-row shift that WorldToCell applies)
	const int32 row = texel.Y - cellOffset.Y;
	const int32 q = (texel.X - cellOffset.X) - FMath::FloorToInt(row * 0.5f);

	//Step to the neighbour, then hex coordinate -> pixel again
	const int32 nq = q + dirs[direction].X;
	const int32 nRow = row + dirs[direction].Y;
	return FIntPoint(nq + FMath::FloorToInt(nRow * 0.5f), nRow) + cellOffset;
}

// Fog: for every pixel, count the tiles to the nearest revealed tile (stops at frontierCap)
void UHexBiomeMaskComponent::UpdateFrontier()
{
	const int32 count = texSize.X * texSize.Y;
	TArray<int32> distance;
	distance.Init(frontierCap, count);

	//Start from every revealed tile at distance 0, then spread outward one ring at a time
	TArray<FIntPoint> queue;
	for (int32 i = 0; i < count; i++)
	{
		if (revealed[i])
		{
			distance[i] = 0;
			queue.Add(FIntPoint(i % texSize.X, i / texSize.X));
		}
	}

	for (int32 head = 0; head < queue.Num(); head++)
	{
		const FIntPoint texel = queue[head];
		const int32 nextDistance = distance[texel.Y * texSize.X + texel.X] + 1;
		if (nextDistance >= frontierCap) continue;

		for (int32 dir = 0; dir < 6; dir++)
		{
			const FIntPoint n = TexelNeighbour(texel, dir);
			if (n.X < 0 || n.Y < 0 || n.X >= texSize.X || n.Y >= texSize.Y) continue;

			const int32 nIndex = n.Y * texSize.X + n.X;
			if (distance[nIndex] <= nextDistance) continue;

			distance[nIndex] = nextDistance;
			queue.Add(n);
		}
	}

	// Fog: store the new distances as targets; Tick eases G toward them. A holds the target for material effects
	for (int32 i = 0; i < count; i++)
	{
		frontierTarget[i] = (float)distance[i];
		fogPixels[i].A = (uint8)(distance[i] * 16);

		//The very first update snaps instead of animating, so the game doesn't start with fog sweeping away
		if (!frontierInitialized)
		{
			frontierSmooth[i] = (float)distance[i];
			fogPixels[i].G = (uint8)(distance[i] * 16);
		}
	}
	frontierInitialized = true;
	frontierAnimating = true;
}


// Fog clouds: find the tagged cloud actor(s) and give each plane its own material instance with the fog data
void UHexBiomeMaskComponent::SetupFogClouds()
{
	TArray<AActor*> cloudActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), fogCloudTag, cloudActors);

	TArray<UStaticMeshComponent*> layers;
	for (AActor* actor : cloudActors)
	{
		TArray<UStaticMeshComponent*> meshes;
		actor->GetComponents<UStaticMeshComponent>(meshes);
		layers.Append(meshes);
	}

	//Lowest plane becomes layer 0, so the material can make each layer look slightly different
	layers.Sort([](const UStaticMeshComponent& a, const UStaticMeshComponent& b)
		{
			return a.GetComponentLocation().Z < b.GetComponentLocation().Z;
		});

	for (int32 i = 0; i < layers.Num(); i++)
	{
		UMaterialInstanceDynamic* material = layers[i]->CreateDynamicMaterialInstance(0);
		if (!material) continue;

		ApplyToMaterial(material);
		material->SetScalarParameterValue(TEXT("LayerIndex"), i);
		material->SetScalarParameterValue(TEXT("LayerCount"), layers.Num());
	}

	UE_LOG(LogTemp, Log, TEXT("HexBiomeMask: %d fog cloud layers found"), layers.Num());
}