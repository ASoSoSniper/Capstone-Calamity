// Fill out your copyright notice in the Description page of Project Settings.


#include "Rwrd_EventScheduled.h"
#include "EventSystemManager.h"

void URwrd_EventScheduled::ApplyReward(UFaction* faction)
{
	AEventSystemManager::ScheduleEvent(daysAhead, FName(eventKey));
}
