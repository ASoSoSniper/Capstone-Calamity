// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_ValidTerrain.h"

float UUTC_ValidTerrain::ScoreCondition(UFaction* faction, ABaseHex* hex) const
{
	return FactorInversion(!invalidTerrain.Contains(hex->GetHexTerrain()));
}
