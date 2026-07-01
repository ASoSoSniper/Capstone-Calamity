// Fill out your copyright notice in the Description page of Project Settings.


#include "UAI_PriorityManager_Troops.h"
#include "Faction.h"
#include "UAI_HexCondition.h"
#include "UAI_TroopCondition.h"
#include "GlobalSpawner.h"

// Sets default values for this component's properties
UUAI_PriorityManager_Troops::UUAI_PriorityManager_Troops()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUAI_PriorityManager_Troops::Initialize(UFaction* faction)
{
	if (parentFaction) return;

	parentFaction = faction;
}

ABaseHex* UUAI_PriorityManager_Troops::GetPriorityHex() const
{
	return priorityHex;
}

ATroop* UUAI_PriorityManager_Troops::GetPriorityTroop() const
{
	return priorityTroop;
}

void UUAI_PriorityManager_Troops::FindPriorityHex()
{
	if (parentFaction->GetTroops().IsEmpty()) return;

	const TArray<TArray<ABaseHex*>>& hexArray = AGlobalSpawner::spawnerObject->hexArray;

	ABaseHex* bestHex = nullptr;
	float bestScore = 0.f;

	for (int x = 0; x < hexArray.Num(); x++)
	{
		for (int y = 0; y < hexArray[x].Num(); y++)
		{
			ABaseHex* hex = hexArray[x][y];
			if (!hex->IsTraversableTerrain()) continue;

			float score = ScoreHex(priorityTroop, hex);
			if (score > bestScore)
			{
				bestScore = score;
				bestHex = hex;
			}
		}
	}

	priorityHex = bestHex;
	FindPriorityTroop();
}

void UUAI_PriorityManager_Troops::FindPriorityTroop()
{
	if (!priorityHex) FindPriorityHex();

	ATroop* bestTroop = nullptr;
	float bestScore = 0.f;

	const TSet<ATroop*>& troops = parentFaction->GetTroops();

	for (ATroop* troop : troops)
	{
		float score = ScoreHex(troop, priorityHex);
		if (score > bestScore)
		{
			bestScore = score;
			bestTroop = troop;
		}
	}

	priorityTroop = bestTroop;
}

void UUAI_PriorityManager_Troops::HandleOnTroopChanged()
{
	const TSet<ATroop*>& troops = parentFaction->GetTroops();

	if (troops.IsEmpty())
		priorityTroop = nullptr;
	else if (troops.Num() == 1) 
		priorityTroop = *troops.CreateConstIterator();
	else 
		FindPriorityTroop();
}

void UUAI_PriorityManager_Troops::HandleOnHexTargeted()
{
	FindPriorityHex();
}

float UUAI_PriorityManager_Troops::ScoreHex(ATroop* troop, ABaseHex* hex)
{
	float score = 1.f;

	if (hexTargetConditions.IsEmpty()) return score;

	for (int i = 0; i < hexTargetConditions.Num(); i++)
	{
		score *= hexTargetConditions[i]->ScoreCondition(troop, hex);

		if (score == 0) return 0;
	}

	float originalScore = score;
	float modFactor = 1 - (1 / hexTargetConditions.Num());
	float makeUpValue = (1 - originalScore) * modFactor;
	return originalScore + (makeUpValue * originalScore);
}

