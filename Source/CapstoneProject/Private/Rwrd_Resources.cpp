// Fill out your copyright notice in the Description page of Project Settings.


#include "Rwrd_Resources.h"
#include "Faction.h"

void URwrd_Resources::ApplyReward(UFaction* faction)
{
	faction->SetResources(resources);
}

FString URwrd_Resources::GetRewardText() const
{
	FString text = "";
	for (const TPair<EStratResources, int32>& r : resources)
	{
		FString resource = "";
		switch (r.Key)
		{
		case EStratResources::Energy:
			resource += TEXT(":energy:");
			break;
		case EStratResources::Production:
			resource += TEXT(":prod:");
			break;
		case EStratResources::Food:
			resource += TEXT(":food:");
			break;
		case EStratResources::Wealth:
			resource += TEXT(":wealth:");
			break;
		case EStratResources::Population:
			resource += TEXT(":pop:");
			break;
		}

		text += FString::Printf(TEXT("<img id=\"%s\"/>%d "), *resource, r.Value);
	}

	return text;
}
