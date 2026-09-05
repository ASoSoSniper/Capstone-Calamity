// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIA_ConstructTroop.h"
#include "FactionController.h"
#include "Faction.h"
#include "TroopFactory.h"

void UUAIA_ConstructTroop::ExecuteAction(IUAI_Controller* controller)
{
	UFaction* faction = controller->GetFaction();
	if (!faction || !constructionAction) return;

	TSet<ABuilding*> factories = faction->GetBuildingsOfType(SpawnableBuildings::RobotFactory);
	if (factories.IsEmpty()) {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No factories found, cannot build troops"));
		return;
	}

	TArray<ABuilding*> factoriesCompleted;
	for (ABuilding* b : factories)
	{
		if (b->ConstructionComplete()) factoriesCompleted.Add(b);
	}
	if (factoriesCompleted.IsEmpty()) {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No factories completed construction, cannot build troops"));
		return;
	}

	int index = FMath::RandRange(0, factoriesCompleted.Num() - 1);
	ABuilding* selectedFactory = factoriesCompleted[index];
	if (!selectedFactory) {
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("No factories selected, cannot build troops"));
		return;
	}

	constructionAction->ExecuteAction(selectedFactory);
}
