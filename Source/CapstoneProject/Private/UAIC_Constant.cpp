// Fill out your copyright notice in the Description page of Project Settings.


#include "UAIC_Constant.h"

float UUAIC_Constant::ScoreCondition(IUAI_Controller* controller) const
{
	return FactorInversion(constantScore);
}
