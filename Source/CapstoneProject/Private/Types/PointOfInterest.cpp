// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/PointOfInterest.h"
#include "CapstoneProjectGameModeBase.h"

FString FPointOfInterest::GetPointTitle() const
{
	return pointTitle;
}
const TMap<EStratResources, int32>& FPointOfInterest::GetRewards() const
{
	return rewards;
}
FString FPointOfInterest::GetWorldDisplay() const
{
	FString message = pointTitle + FString::Printf(TEXT(" - <img id=\":time:\"/> %d day%s\n"), daysRemaining,
		daysRemaining > 1 ? TEXT("s") : TEXT(""));

	for (const TPair<EStratResources, int32>& r : rewards)
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

		message += FString::Printf(TEXT("<img id=\"%s\"/>%d "), *resource, r.Value);
	}

	return message;
}
int32 FPointOfInterest::GetDaysToComplete() const
{
	return daysRemaining;
}
bool FPointOfInterest::Work()
{
	if (WorkCompleted()) return false;

	if (!ACapstoneProjectGameModeBase::GetDateUpdates()->minuteTick) return false;

	minutes++;
	if (minutes < 2) return false;

	hoursRemaining++;
	minutes = 0;
	if (hoursRemaining < 24) return false;

	daysRemaining--;
	hoursRemaining = 0;

	return WorkCompleted();
}
bool FPointOfInterest::WorkCompleted() const
{
	return daysRemaining <= 0;
}
float FPointOfInterest::GetWorkProgress() const
{
	long int daysToMinutes = (daysToComplete - daysRemaining) * 1440;
	long int hoursToMinutes = hoursRemaining * 60;

	long int totalMinutes = daysToComplete * 1440;
	long int currentMinutes = minutes + hoursToMinutes + daysToMinutes;

	return static_cast<float>(currentMinutes) / static_cast<float>(totalMinutes);
}