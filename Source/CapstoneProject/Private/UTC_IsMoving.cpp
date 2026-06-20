// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_IsMoving.h"

float UUTC_IsMoving::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
    return FactorInversion(troop->IsTraveling());
}
