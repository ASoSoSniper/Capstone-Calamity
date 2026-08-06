// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_TroopsAvailable.h"
#include "Faction.h"

float UUAIC_TroopsAvailable::ScoreCondition(IUAI_Controller* controller) const
{
	int troopCount = controller->GetFaction()->GetTroops().Num();

	float alpha = static_cast<float>(troopCount) / maxCount;

	return EvaluateOnCurve(FactorInversion(alpha));
}
