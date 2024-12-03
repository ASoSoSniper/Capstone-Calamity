// Fill out your copyright notice in the Description page of Project Settings.


#include "Faction_AIController.h"
#include "FactionAction.h"

Faction_AIController::Faction_AIController(Faction* faction)
{
	parentFaction = faction;

	GeneratePersonality();

}

Faction_AIController::~Faction_AIController()
{
}

Faction* Faction_AIController::GetFaction()
{
	return parentFaction;
}

void Faction_AIController::SelectAction()
{
	int selectedAction = 0;
	int highScore = 0;

	for (int i = 0; i < actions.Num(); i++)
	{
		if (!actions[i]) continue;

		int score = actions[i]->ScoreAction(*this);

		if (score > highScore)
		{
			highScore = score;
			selectedAction = i;
		}
	}

	actions[selectedAction]->PerformAction(*this);
}

void Faction_AIController::GeneratePersonality()
{
	personality.xenophilia = FMath::RandRange(-100, 100);
	personality.spirituality = FMath::RandRange(-100, 100);
	personality.compliance = FMath::RandRange(-100, 100);
	personality.violence = FMath::RandRange(-100, 100);
}
