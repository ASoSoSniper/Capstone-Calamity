// Fill out your copyright notice in the Description page of Project Settings.


#include "UHC_AvailableRadius.h"

float UUHC_AvailableRadius::ScoreCondition(ABaseHex* hex) const
{
	return FactorInversion(hex->CanBuildOnHex(radius));
}
