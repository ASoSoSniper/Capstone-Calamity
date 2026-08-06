// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_TroopCommandable.h"
#include "Faction.h"
#include "Troop.h"
#include "UAI_TroopCondition.h"

float UUAIC_TroopCommandable::ScoreCondition(IUAI_Controller* controller) const
{
	if (commandableConditions.IsEmpty()) return FactorInversion(1.f);

	const TSet<ATroop*>& troops = controller->GetFaction()->GetTroops();
	float minSuccessScore = minSuccessCondition ? minSuccessCondition->ScoreCondition(controller) : staticMinSuccess;

	for (ATroop* troop : troops)
	{
		float score = 1.f;

		for (int i = 0; i < commandableConditions.Num(); i++)
		{
			float conditionScore = commandableConditions[i]->ScoreCondition(troop, nullptr);
			score *= conditionScore;

			if (score == 0) break;
		}

		float modFactor = 1 - (1 / commandableConditions.Num());
		float makeUpValue = (1 - score) * modFactor;
		float totalScore = score + (makeUpValue * score);

		if (totalScore >= minSuccessScore) return FactorInversion(1.f);
	}

	return FactorInversion(0.f);
}
