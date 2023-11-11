// Fill out your copyright notice in the Description page of Project Settings.


#include "OutpostStorage.h"

void UOutpostStorage::UpdateResources()
{
	Cast<ABaseHex>(outpost->hexNav->currentHex)->UpdateResourceCapIncrease(storageIncrease);
}
