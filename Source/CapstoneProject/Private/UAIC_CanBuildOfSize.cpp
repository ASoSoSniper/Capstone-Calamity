// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_CanBuildOfSize.h"
#include "FactionController.h"
#include "Faction.h"

float UUAIC_CanBuildOfSize::ScoreCondition(IUAI_Controller* controller) const
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController) return GetMinScore();

	UFaction* faction = factionController->GetFactionObject();
	if (!faction) return GetMinScore();

	for (ABaseHex*& hex : faction->ownedHexes)
	{
		if (hex->CanBuildOnHex(buildingSize)) return FactorInversion(1.f);
	}

	return GetMinScore();
}
