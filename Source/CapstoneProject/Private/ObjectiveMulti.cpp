// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveMulti.h"

FString UObjectiveMulti::GetObjectiveDisplay() const
{
	FString display = GetObjectiveDescription();
	for (int i = 0; i < subObjectives.Num(); i++)
	{
		display += TEXT("\n   - ") + subObjectives[i]->GetObjectiveDescription();
	}

	return display;
}

void UObjectiveMulti::SetupObjective(UFaction* faction)
{
	Super::SetupObjective(faction);

	for (int i = 0; i < subObjectives.Num(); i++)
	{
		complete.Add(subObjectives[i], false);
		subObjectives[i]->onObjectiveComplete.AddDynamic(this, &UObjectiveMulti::HandleOnCompleted);

		if (completeInOrder && i > 0) continue;
		subObjectives[i]->SetupObjective(faction);
	}
}

void UObjectiveMulti::CleanupObjective()
{
	Super::CleanupObjective();

	complete.Empty();
	index = 0;
}

void UObjectiveMulti::HandleOnCompleted(UObjective* subObjective)
{
	complete[subObjective] = true;

	bool allCompleted = true;
	for (const TPair<UObjective*, bool>& objective : complete)
	{
		if (!objective.Value)
		{
			allCompleted = false;
			break;
		}
	}

	if (allCompleted)
	{
		CompleteObjective();
	}
	else if (completeInOrder)
	{
		index = FMath::Min(index + 1, subObjectives.Num() - 1);
		subObjectives[index]->SetupObjective(factionObject);
	}
}
