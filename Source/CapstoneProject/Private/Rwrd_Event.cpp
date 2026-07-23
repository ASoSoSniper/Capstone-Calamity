// Fill out your copyright notice in the Description page of Project Settings.


#include "Rwrd_Event.h"
#include "EventSystemManager.h"

void URwrd_Event::ApplyReward(UFaction* faction)
{
	AEventSystemManager::TriggerEvent(FName(eventKey));
}

FString URwrd_Event::GetRewardText() const
{
	FWorldEvent* event = AEventSystemManager::GetEvent(FName(eventKey));
	if (!event) return TEXT("NONEXISTENT EVENT");

	return event->title;
}
