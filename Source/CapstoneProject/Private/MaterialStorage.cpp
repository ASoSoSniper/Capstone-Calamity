// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialStorage.h"

void AMaterialStorage::UpdateResources()
{
	Cast<ABaseHex>(hexNav->currentHex)->UpdateResourceCapIncrease(storageIncrease);
}
