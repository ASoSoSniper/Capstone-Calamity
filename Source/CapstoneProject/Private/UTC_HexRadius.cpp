// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_HexRadius.h"

float UUTC_HexRadius::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	return FactorInversion(hex->CanBuildOnHex(radius));
}
