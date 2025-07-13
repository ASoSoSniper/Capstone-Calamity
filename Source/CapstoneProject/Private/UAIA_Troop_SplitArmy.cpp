// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_Troop_SplitArmy.h"
#include "MergedArmy.h"

void UUAIA_Troop_SplitArmy::ExecuteAction(IUAI_Controller* controller)
{
	AMergedArmy* army = Cast<AMergedArmy>(controller);
	if (!army) return;

	switch (splitType)
	{
	case ESplitType::InHalf:
		army->SplitInHalf();
		break;
	case ESplitType::OneUnit:
		army->ExtractOneUnit(unitToSplit);
		break;
	}
}
