// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_ConstructTroop.h"
#include "FactionController.h"
#include "Faction.h"
#include "TroopFactory.h"

void UUAIA_ConstructTroop::ExecuteAction(IUAI_Controller* controller)
{
	AFactionController* factionController = Cast<AFactionController>(controller);
	if (!factionController || !constructionAction) return;

	TSet<ABuilding*> factories = factionController->GetFactionObject()->GetBuildingsOfType(SpawnableBuildings::RobotFactory);
	if (factories.IsEmpty()) return;

	ATroopFactory* selectedFactory = nullptr;
	int index = FMath::RandRange(0, factories.Num() - 1);
	int i = 0;
	for (ABuilding* b : factories)
	{
		if (i++ == index)
			selectedFactory = Cast<ATroopFactory>(b);
	}
	if (!selectedFactory) return;

	constructionAction->ExecuteAction(selectedFactory);
}
