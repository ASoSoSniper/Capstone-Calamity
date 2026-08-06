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
	TArray<FString> bestConditionScores;

	for (int x = 0; x < hexArray.Num(); x++)
	{
		for (int y = 0; y < hexArray[x].Num(); y++)
		{
			ABaseHex* hex = hexArray[x][y];
			if (!hex->IsTraversableTerrain()) continue;

			TArray<FString> conditionScores;

			float score = ScoreHex(priorityTroop, hex, conditionScores);
			if (score > bestScore)
			{
				bestScore = score;
				bestHex = hex;
				bestConditionScores = conditionScores;
			}
		}
	}

	priorityHex = bestHex;
	if (!priorityHex) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ERROR: No Priority Hex Set"));
		return;
	}
	onHexSelected.Broadcast(priorityHex, bestConditionScores);
	FindPriorityTroop();
}

void UUAI_PriorityManager_Troops::FindPriorityTroop()
{
	if (!priorityHex)
	{
		FindPriorityHex();
		return;
	}

	ATroop* bestTroop = nullptr;
	float bestScore = 0.f;
	TArray<FString> bestConditionScores;

	const TSet<ATroop*>& troops = parentFaction->GetTroops();

	for (ATroop* troop : troops)
	{
		TArray<FString> conditionScores;

		float score = ScoreTroop(troop, priorityHex, conditionScores);
		if (score > bestScore)
		{
			bestScore = score;
			bestTroop = troop;
			bestConditionScores = conditionScores;
		}
	}

	if (!bestTroop) return;

	priorityTroop = bestTroop;
	onTroopSelected.Broadcast(priorityTroop, bestConditionScores);
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

float UUAI_PriorityManager_Troops::ScoreHex(ATroop* troop, ABaseHex* hex, TArray<FString>& outScores)
{
	return Score(hexTargetConditions, troop, hex, outScores);
}

float UUAI_PriorityManager_Troops::ScoreTroop(ATroop* troop, ABaseHex* hex, TArray<FString>& outScores)
{
	return Score(troopToTargetConditions, troop, hex, outScores);
}

float UUAI_PriorityManager_Troops::Score(const TArray<UAI_TroopCondition*>& conditions, ATroop* troop, ABaseHex* hex, TArray<FString>& outScores)
{
	float score = 1.f;

	if (conditions.IsEmpty()) return score;

	for (int i = 0; i < conditions.Num(); i++)
	{
		float conditionScore = conditions[i]->ScoreCondition(troop, hex);
		score *= conditionScore;

		outScores.Add(FString::Printf(TEXT("%s: %.2f"), *conditions[i]->GetName(), conditionScore));

		if (score == 0) return 0;
	}

	float originalScore = score;
	float modFactor = 1 - (1 / conditions.Num());
	float makeUpValue = (1 - originalScore) * modFactor;
	return originalScore + (makeUpValue * originalScore);
}

