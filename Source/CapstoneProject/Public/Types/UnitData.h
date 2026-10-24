// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/StatusEffect.h"
#include "FactionEnum.h"
#include "UnitActions.h"
#include "UnitData.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FUnitData
{
	GENERATED_USTRUCT_BODY()

public:
	FUnitData() : faction(EFactions::None), unitType(UnitTypes::None) {};
	FUnitData(EFactions setFaction) : faction(setFaction), unitType(UnitTypes::None)
	{
		FUnitData(faction, UnitTypes::None);
	};
	FUnitData(EFactions setFaction, UnitTypes setUnitType) : faction(setFaction), unitType(setUnitType)
	{
		if (unitType == UnitTypes::Army)
			GenerateArmyName();
	}

	TMap<UnitTypes, FUnitComposition> GetUnitComposition() const;
	UnitTypes GetLargestUnitQuantity() const;

	EFactions GetFaction() const;
	UnitTypes GetUnitType() const;

	void SetUnitValues(int setHealth, int setMorale, int setVision, int setSpeed, int setDamage, int setSiegePower, int setReinforceRate, int setEnergyUpKeep);
	void AddUnitData(FUnitData* data);
	FUnitData* ExtractUnitData(int32 index, bool killOnExtraction = false);
	void SetBuildingValues(int setHealth, int setVision, int setSiegePower, int setEnergyUpkeep);

	void GenerateArmyName(FString newName = TEXT(""));
	FText GetArmyName() const;
	FString GetNameRaw() const;
	int GetNameInstance() const;

	int GetCurrentHP() const;
	int GetMaxHP() const;
	void SetHPByAlpha(float alpha, bool allowDeath = true);
	int GetCurrentMorale() const;
	int GetMaxMorale() const;
	void SetMoraleByAlpha(float alpha);
	void SetMoraleByValue(int value);
	float GetHPAlpha() const;
	float GetMoraleAlpha() const;
	bool IsAlive() const;
	bool HasMorale() const;

	int DamageHP(int amount);
	int DamageMorale(int amount);
	int HealHP(int amount);

	int GetVision() const;
	int GetSpeed() const;

	int GetDamage() const;
	int GetSiegePower() const;

	int GetReinforceRate() const;
	int GetEnergyUpkeep() const;

	bool IsBuilding() const;

	const TArray<FUnitData*>& GetSavedUnits() const;
	int GetSavedUnitCount() const;

	bool SetupComplete() const;
	void DestroyWorldData();

	const TSet<FStatusEffect*>& GetStatusEffects() const;
	void AddStatusEffect(FStatusEffect* effect);
	void RemoveStatusEffect(FStatusEffect* effect);
	void ClearStatusEffects();

private:
	EFactions faction;
	UnitTypes unitType;

	FString armyName = TEXT("");
	int nameInstance = 0;

	int currentHP = 0;
	int maxHP = 0;
	int currentMorale = 0;
	int maxMorale = 0;

	int vision = 0;
	int speed = 0;

	int damage = 0;
	int siegePower = 0;

	int reinforceRate = 0;
	int energyUpkeep = 0;

	bool isBuilding = false;

	TArray<FUnitData*> savedUnits;
	TSet<FStatusEffect*> statusEffects;

	void RemoveArmyName();
};