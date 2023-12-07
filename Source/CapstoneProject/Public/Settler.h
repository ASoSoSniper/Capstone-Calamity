// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Troop.h"
#include "Settler.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ASettler : public ATroop
{
	GENERATED_BODY()
	
public:
	int popInStorage = 0;
};
