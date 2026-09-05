// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_Composite.h"

float UUAIC_Composite::ScoreCondition(IUAI_Controller* controller) const
{
	float totalScore = 0.f;

	for (int i = 0; i < conditions.Num(); i++)
	{
		float score = conditions[i]->ScoreCondition(controller);
		totalScore += score;

		if (allMustBeTrue)
		{
			if (score < minScoreForSuccess) return GetMinScore();
		}
		else
		{
			if (score >= minScoreForSuccess && !useAverageScore)
			{
				return EvaluateOnCurve(FactorInversion(scoreOnSuccess));
			}
		}
	}

	if (!useAverageScore && !allMustBeTrue) return GetMinScore();
	if (!useAverageScore) return EvaluateOnCurve(FactorInversion(scoreOnSuccess));

	return EvaluateOnCurve(FactorInversion(totalScore / conditions.Num()));
}
