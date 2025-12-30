// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_Composite.h"

float UUAIC_Composite::ScoreCondition(IUAI_Controller* controller) const
{
	float finalScore = EvaluateOnCurve(FactorInversion(scoreOnSuccess));

	for (int i = 0; i < conditions.Num(); i++)
	{
		float score = conditions[i]->ScoreCondition(controller);
		if (allMustBeTrue)
		{
			if (score < minScoreForSuccess) return GetMinScore();
		}
		else
		{
			if (score >= minScoreForSuccess) return finalScore;
		}
	}

	return allMustBeTrue ? finalScore : GetMinScore();
}
