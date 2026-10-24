// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RandomInt.h"
#include "HexInfo.generated.h"

USTRUCT(BlueprintType)
struct FHexInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true")) FString description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRandomInt food;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRandomInt production;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRandomInt energy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRandomInt wealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defenderBonus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float moveMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float attritionMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int visionModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool canSeeInto;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int startingSeedSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* model;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* topper;
};

USTRUCT(BlueprintType)
struct FHexInstance
{
	GENERATED_USTRUCT_BODY()

	FHexInstance() :
		name(FText()),
		description(FText()),
		icon(nullptr),
		food(FMath::RandRange(0, 0)),
		production(FMath::RandRange(0, 0)),
		energy(FMath::RandRange(0, 0)),
		wealth(FMath::RandRange(0, 0)),
		defenderBonus(0),
		moveMultiplier(0),
		attritionMultiplier(0),
		visionModifier(0),
		canSeeInto(false) {}

	FHexInstance(const FHexInfo& sourceInfo) : 
		name(FText::FromString(sourceInfo.name)), 
		description(FText::FromString(sourceInfo.description)),
		icon(sourceInfo.icon),
		food(FMath::RandRange(sourceInfo.food.randomMin, sourceInfo.food.randomMax)), 
		production(FMath::RandRange(sourceInfo.production.randomMin, sourceInfo.production.randomMax)),
		energy(FMath::RandRange(sourceInfo.energy.randomMin, sourceInfo.energy.randomMax)),
		wealth(FMath::RandRange(sourceInfo.wealth.randomMin, sourceInfo.wealth.randomMax)),
		defenderBonus(sourceInfo.defenderBonus),
		moveMultiplier(sourceInfo.moveMultiplier),
		attritionMultiplier(sourceInfo.attritionMultiplier),
		visionModifier(sourceInfo.visionModifier),
		canSeeInto(sourceInfo.canSeeInto){}

	UPROPERTY(BlueprintReadOnly) FText name = FText();
	UPROPERTY(BlueprintReadOnly) FText description = FText();
	UPROPERTY(BlueprintReadOnly) UTexture2D* icon = nullptr;

	UPROPERTY(BlueprintReadOnly) int food = 0;
	UPROPERTY(BlueprintReadOnly) int production = 0;
	UPROPERTY(BlueprintReadOnly) int energy = 0;
	UPROPERTY(BlueprintReadOnly) int wealth = 0;

	UPROPERTY(BlueprintReadOnly) int defenderBonus = 0;

	UPROPERTY(BlueprintReadOnly) float moveMultiplier = 0.f;
	UPROPERTY(BlueprintReadOnly) float attritionMultiplier = 0.f;
	UPROPERTY(BlueprintReadOnly) int visionModifier = 0;
	UPROPERTY(BlueprintReadOnly) bool canSeeInto = true;
};