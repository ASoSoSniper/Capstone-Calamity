// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_PriorityManager_Hex.h"
#include "GlobalSpawner.h"

UUAI_PriorityManager_Hex::UUAI_PriorityManager_Hex()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUAI_PriorityManager_Hex::Initialize(UFaction* faction)
{
	if (faction == nullptr) return;

	factionParent = faction;
	factionHexes = &faction->GetOwnedHexes();

	FindPriorityHex_Workers_All();
	FindPriorityHex_Buildings_All();
}

ABaseHex* UUAI_PriorityManager_Hex::GetPriorityHex_Building(SpawnableBuildings building) const
{
	if (!priorityHexes_Building.Contains(building)) return nullptr;

	return priorityHexes_Building[building];
}

ABaseHex* UUAI_PriorityManager_Hex::GetPriorityHex_Workers(EStratResources resource) const
{
	if (!priorityHexes_Workers.Contains(resource)) return nullptr;

	return priorityHexes_Workers[resource];
}

void UUAI_PriorityManager_Hex::FindPriorityHex_Building(SpawnableBuildings building)
{
	if (factionHexes->IsEmpty()) return;

	if (!priorityHexes_Building.Contains(building))
		priorityHexes_Building.Add(building, nullptr);

	if (!buildingSearchConditions.Contains(building) || 
		buildingSearchConditions[building].conditions.IsEmpty())
	{
		EStratResources resource = AGlobalSpawner::GetMainBuildingYield(building);
		if (priorityHexes_Workers.Contains(resource))
		{
			priorityHexes_Building[building] = priorityHexes_Workers[resource];
			return;
		}
	}

	ABaseHex* bestHex = nullptr;
	float bestScore = 0.f;

	for (const TPair<TerrainType, FHexSet>& terrains : *factionHexes)
	{
		if (!ABaseHex::IsBuildableTerrain(terrains.Key)) continue;

		for (ABaseHex* hex : terrains.Value.hexes)
		{
			if (hex->building) continue;

			float score = ScoreHex(building, hex);
			if (!bestHex || score > bestScore)
			{
				bestScore = score;
				bestHex = hex;
			}
		}
	}

	priorityHexes_Building[building] = bestHex;
}

void UUAI_PriorityManager_Hex::FindPriorityHex_Workers(EStratResources resource)
{
	if (factionHexes->IsEmpty()) return;

	if (!priorityHexes_Workers.Contains(resource)) 
		priorityHexes_Workers.Add(resource, nullptr);

	ABaseHex* bestHex = nullptr;
	int bestResource = 0;

	for (const TPair<TerrainType, FHexSet>& terrains : *factionHexes)
	{
		if (terrains.Key == TerrainType::Mountains || terrains.Key == TerrainType::Border) continue;

		for (ABaseHex* hex : terrains.Value.hexes)
		{
			if (hex->workersInHex[WorkerType::Alien] >= hex->GetMaxWorkers()) continue;
			int resourceInHex = hex->GetHexResources()[resource];

			if (!bestHex || resourceInHex > bestResource)
			{
				bestHex = hex;
				bestResource = resourceInHex;
			}
		}
	}

	priorityHexes_Workers[resource] = bestHex;
}

void UUAI_PriorityManager_Hex::FindPriorityHex_Buildings_All()
{
	for (int i = 0; i < static_cast<int>(SpawnableBuildings::Outpost); i++)
	{
		SpawnableBuildings sb = static_cast<SpawnableBuildings>(i);
		if (sb == SpawnableBuildings::None) continue;

		FindPriorityHex_Building(sb);
	}
}

void UUAI_PriorityManager_Hex::FindPriorityHex_Workers_All()
{
	for (int i = 0; i < static_cast<int>(EStratResources::Population); i++)
	{
		EStratResources sr = static_cast<EStratResources>(i);
		if (sr == EStratResources::Population || sr == EStratResources::None) continue;

		FindPriorityHex_Workers(sr);
	}
}

void UUAI_PriorityManager_Hex::BindHexDelegates(ABaseHex* hex, bool enable)
{
	if (enable)
	{
		hex->onBuildingSet.AddDynamic(this, &UUAI_PriorityManager_Hex::HandleOnBuildingSet);
		hex->onWorkersSet.AddDynamic(this, &UUAI_PriorityManager_Hex::HandleOnWorkersSet);
	}
	else
	{
		hex->onBuildingSet.RemoveDynamic(this, &UUAI_PriorityManager_Hex::HandleOnBuildingSet);
		hex->onWorkersSet.RemoveDynamic(this, &UUAI_PriorityManager_Hex::HandleOnWorkersSet);
	}
}

void UUAI_PriorityManager_Hex::BindBuildingDelegates(ABuilding* building, bool enable)
{
	if (enable)
	{
		building->onHexesClaimed.AddDynamic(this, &UUAI_PriorityManager_Hex::HandleOnHexesClaimed);
	}
	else
	{
		building->onHexesClaimed.RemoveDynamic(this, &UUAI_PriorityManager_Hex::HandleOnHexesClaimed);
	}
}

void UUAI_PriorityManager_Hex::HandleOnBuildingSet(ABaseHex* hex)
{
	if (!hex->building)
	{
		FindPriorityHex_Buildings_All();
		return;
	}

	FindPriorityHex_Building(hex->building->GetBuildingType());
}

void UUAI_PriorityManager_Hex::HandleOnWorkersSet(ABaseHex* hex)
{
	if (hex->WorkersAtCapacity()) 
		FindPriorityHex_Workers(hex->GetMainResourceYield());
}

void UUAI_PriorityManager_Hex::HandleOnHexesClaimed()
{
	FindPriorityHex_Workers_All();
	FindPriorityHex_Buildings_All();
}

float UUAI_PriorityManager_Hex::ScoreHex(SpawnableBuildings building, ABaseHex* hex)
{
	float score = 1.f;

	if (!buildingSearchConditions.Contains(building)) return score;

	const TArray<UAI_HexCondition*>& conditions = buildingSearchConditions[building].conditions;
	if (conditions.IsEmpty()) return score;

	for (int i = 0; i < conditions.Num(); i++)
	{
		score *= conditions[i]->ScoreCondition(hex);

		if (score == 0) return 0;
	}

	float originalScore = score;
	float modFactor = 1 - (1 / conditions.Num());
	float makeUpValue = (1 - originalScore) * modFactor;
	return originalScore + (makeUpValue * originalScore);
}
