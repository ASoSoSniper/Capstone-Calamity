// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienCity.h"
#include "CityEmbassy.h"
#include "CityPoliceStation.h"
#include "CityTradeOutpost.h"
#include "BuildingAttachment.h"
#include "OutpostStorage.h"
#include "OutpostBarracks.h"
#include "OutpostTroopFactory.h"
#include "OutpostDefenses.h"

AAlienCity::AAlienCity()
{
	cityEmbassy = CreateDefaultSubobject<UCityEmbassy>(TEXT("Embassy"));
	cityPoliceStation = CreateDefaultSubobject<UCityPoliceStation>(TEXT("Police Station"));
	cityTradeOutpost = CreateDefaultSubobject<UCityTradeOutpost>(TEXT("Trade Outpost"));
}

UBuildingAttachment* AAlienCity::GetAttachment(BuildingAttachments attachment)
{
	if (UBuildingAttachment* attachmentType = Super::GetAttachment(attachment))
	{
		return attachmentType;
	}

	switch (attachment)
	{
	case BuildingAttachments::Embassy:
		return cityEmbassy;
	case BuildingAttachments::PoliceStation:
		return cityPoliceStation;
	case BuildingAttachments::TradeOutpost:
		return cityTradeOutpost;
	default:
		return nullptr;
	}
}
