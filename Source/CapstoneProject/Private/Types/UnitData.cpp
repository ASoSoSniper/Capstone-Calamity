// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/UnitData.h"
#include "Faction.h"
#include "Managers/FactionManager.h"
#include "GlobalSpawner.h"

EFactions FUnitData::GetFaction() const
{
	return faction;
}
UnitTypes FUnitData::GetUnitType() const
{
	return unitType;
}

void FUnitData::SetUnitValues(int setHealth, int setMorale, int setVision, int setSpeed, int setDamage, int setSiegePower, int setReinforceRate, int setEnergyUpKeep)
{
	currentHP = setHealth;
	maxHP = setHealth;
	currentMorale = setMorale;
	maxMorale = setMorale;

	vision = setVision;
	speed = setSpeed;

	damage = setDamage;
	siegePower = setSiegePower;

	reinforceRate = setReinforceRate;
	energyUpkeep = setEnergyUpKeep;
}
void FUnitData::AddUnitData(FUnitData* data)
{
	if (unitType != UnitTypes::Army) return;

	if (data->GetUnitType() == UnitTypes::Army)
	{
		TArray<FUnitData*> units = data->GetSavedUnits();
		for (int i = 0; i < units.Num(); i++)
		{
			AddUnitData(units[i]);
		}

		return;
	}

	for (FStatusEffect* effect : data->GetStatusEffects())
	{
		AddStatusEffect(effect);
	}

	savedUnits.Add(data);
	data->DestroyWorldData();

	currentHP += data->currentHP;
	maxHP += data->maxHP;
	currentMorale += data->currentMorale;
	maxMorale += data->maxMorale;

	int setSpeed = 0;
	int bestVision = 0;
	for (int i = 0; i < savedUnits.Num(); i++)
	{
		setSpeed += savedUnits[i]->speed;

		if (savedUnits[i]->vision > bestVision)
			bestVision = savedUnits[i]->vision;
	}
	setSpeed = FMath::RoundToInt((float)setSpeed / (float)savedUnits.Num());

	speed = setSpeed;
	vision = bestVision;

	damage += data->damage;
	siegePower += data->siegePower;

	reinforceRate += data->reinforceRate;
	energyUpkeep += data->energyUpkeep;
}
FUnitData* FUnitData::ExtractUnitData(int32 index, bool killOnExtraction)
{
	if (!savedUnits.IsValidIndex(index)) return nullptr;

	FUnitData* unit = savedUnits[index];
	savedUnits.RemoveAt(index);

	float hpPercent = (float)currentHP / (float)maxHP;
	float moralePercent = (float)currentMorale / (float)maxMorale;

	maxHP -= unit->maxHP;
	currentHP = FMath::RoundToInt((float)maxHP * hpPercent);
	maxMorale -= unit->maxMorale;
	currentMorale = FMath::RoundToInt((float)maxMorale * moralePercent);

	int setSpeed = 0;
	int bestVision = 0;
	for (int i = 0; i < savedUnits.Num(); i++)
	{
		setSpeed += savedUnits[i]->speed;

		if (savedUnits[i]->vision > bestVision)
			bestVision = savedUnits[i]->vision;
	}
	setSpeed = FMath::RoundToInt((float)setSpeed / (float)savedUnits.Num());

	speed = setSpeed;
	vision = bestVision;

	damage -= unit->damage;
	siegePower -= unit->siegePower;

	reinforceRate -= unit->reinforceRate;
	energyUpkeep -= unit->energyUpkeep;

	if (killOnExtraction)
	{
		return nullptr;
	}
	else
	{
		unit->currentHP = FMath::RoundToInt((float)unit->maxHP * hpPercent);
		unit->currentMorale = FMath::RoundToInt((float)unit->maxMorale * hpPercent);
		GenerateArmyName();
	}

	return unit;
}

void FUnitData::SetBuildingValues(int setHealth, int setVision, int setSiegePower, int setEnergyUpkeep)
{
	currentHP = setHealth;
	maxHP = setHealth;

	vision = setVision;

	damage = setSiegePower;
	siegePower = setSiegePower;
	energyUpkeep = setEnergyUpkeep;

	isBuilding = true;
}

void FUnitData::GenerateArmyName(FString newName)
{
	UFaction* factionObject = AFactionManager::GetFactionObject(faction);

	if (!factionObject) return;
	if (armyName == newName && newName != TEXT("")) return;

	TMap<FString, TArray<int32>>& names = faction == EFactions::Human ?
		factionObject->armyNamesHuman :
		factionObject->armyNamesAlien;

	int32& cap = factionObject->maxNameShare;

	RemoveArmyName();
	if (!names.Contains(newName)) names.Add(newName, TArray<int32>());

	if (newName != TEXT(""))
	{
		if (names[newName].Num() < cap)
		{
			int32 num = 0;
			for (int i = 1; i < cap; i++)
			{
				if (!names[newName].Contains(i))
				{
					num = i;
					names[newName].Add(i);
					break;
				}
			}

			armyName = newName;
			nameInstance = num;
			if (num != 0) return;
		}
	}

	for (auto& name : names)
	{
		if (name.Value.Num() < cap)
		{
			int32 num = 0;
			for (int i = 1; i < cap; i++)
			{
				if (!name.Value.Contains(i))
				{
					num = i;
					name.Value.Add(i);
					break;
				}
			}

			armyName = name.Key;
			nameInstance = num;

			if (num != 0) break;
		}
	}
}
void FUnitData::RemoveArmyName()
{
	UFaction* factionObject = AFactionManager::GetFactionObject(faction);
	if (!factionObject) return;

	TMap<FString, TArray<int32>>& names = faction == EFactions::Human ?
		factionObject->armyNamesHuman :
		factionObject->armyNamesAlien;

	if (names.Contains(armyName) && names[armyName].Contains(nameInstance))
	{
		names[armyName].Remove(nameInstance);
	}
}

FText FUnitData::GetArmyName() const
{
	if (unitType != UnitTypes::Army)
		return AGlobalSpawner::spawnerObject->troopStats[unitType].title;

	return FText::FromString(armyName + TEXT(" No. ") + FString::FromInt(nameInstance));
}
FString FUnitData::GetNameRaw() const
{
	return armyName;
}
int FUnitData::GetNameInstance() const
{
	return nameInstance;
}

int FUnitData::GetCurrentHP() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetHPMod();
	}

	return FMath::RoundToInt(static_cast<float>(currentHP) * totalMulti);
}
int FUnitData::GetMaxHP() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetHPMod();
	}

	return FMath::RoundToInt(static_cast<float>(maxHP) * totalMulti);
}
void FUnitData::SetHPByAlpha(float alpha, bool allowDeath)
{
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	currentHP = FMath::RoundToInt((float)maxHP * alpha);

	currentHP = FMath::Max(currentHP, allowDeath ? 0 : 1);
}
int FUnitData::GetCurrentMorale() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetMoraleMod();
	}

	return FMath::RoundToInt(static_cast<float>(currentMorale) * totalMulti);
}
int FUnitData::GetMaxMorale() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetMoraleMod();
	}

	return FMath::RoundToInt(static_cast<float>(maxMorale) * totalMulti);
}
void FUnitData::SetMoraleByAlpha(float alpha)
{
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	currentMorale = FMath::RoundToInt(static_cast<float>(maxMorale) * alpha);
}
void FUnitData::SetMoraleByValue(int value)
{
	currentMorale = FMath::Clamp(value, 0, maxMorale);
}
float FUnitData::GetHPAlpha() const
{
	float curr = static_cast<float>(GetCurrentHP());
	float max = static_cast<float>(GetMaxHP());

	return FMath::Clamp(curr / max, 0.f, 1.f);
}
float FUnitData::GetMoraleAlpha() const
{
	float curr = static_cast<float>(GetCurrentMorale());
	float max = static_cast<float>(GetMaxMorale());

	return FMath::Clamp(curr / max, 0.f, 1.f);
}
bool FUnitData::IsAlive() const
{
	return currentHP > 0;
}
bool FUnitData::HasMorale() const
{
	return currentMorale > 0;
}

int FUnitData::DamageHP(int amount)
{
	int hp = GetCurrentHP();
	int max = GetMaxHP();

	hp -= amount;
	float alpha = static_cast<float>(hp) / static_cast<float>(max);

	SetHPByAlpha(alpha);

	return hp;
}
int FUnitData::DamageMorale(int amount)
{
	int morale = GetCurrentMorale();
	int max = GetMaxMorale();

	morale -= amount;
	float alpha = static_cast<float>(morale) / static_cast<float>(max);

	SetMoraleByAlpha(alpha);

	return morale;
}

int FUnitData::HealHP(int amount)
{
	currentHP += amount;
	currentHP = FMath::Clamp(currentHP, 0, maxHP);

	return currentHP;
}

int FUnitData::GetVision() const
{
	int totalVisionMod = 0;

	for (FStatusEffect* effect : statusEffects)
	{
		totalVisionMod += effect->GetVisionMod();
	}

	return vision + totalVisionMod;
}
int FUnitData::GetSpeed() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetSpeedMod();
	}

	return FMath::RoundToInt(static_cast<float>(speed) * totalMulti);
}
int FUnitData::GetDamage() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetDamageMod();
	}

	return FMath::RoundToInt(static_cast<float>(damage) * totalMulti);
}
int FUnitData::GetSiegePower() const
{
	float totalMulti = 1.f;

	for (FStatusEffect* effect : statusEffects)
	{
		totalMulti *= effect->GetSiegePowerMod();
	}

	return FMath::RoundToInt(static_cast<float>(siegePower) * totalMulti);
}
int FUnitData::GetReinforceRate() const
{
	return reinforceRate;
}
int FUnitData::GetEnergyUpkeep() const
{
	return energyUpkeep;
}
bool FUnitData::IsBuilding() const
{
	return isBuilding;
}
const TArray<FUnitData*>& FUnitData::GetSavedUnits() const
{
	return savedUnits;
}
int FUnitData::GetSavedUnitCount() const
{
	return savedUnits.Num();
}

bool FUnitData::SetupComplete() const
{
	return maxHP > 0;
}
void FUnitData::DestroyWorldData()
{
	RemoveArmyName();
	ClearStatusEffects();
}

const TSet<FStatusEffect*>& FUnitData::GetStatusEffects() const
{
	return statusEffects;
}
void FUnitData::AddStatusEffect(FStatusEffect* effect)
{
	statusEffects.Add(effect);
}
void FUnitData::RemoveStatusEffect(FStatusEffect* effect)
{
	if (!statusEffects.Contains(effect)) return;

	statusEffects.Remove(effect);
}
void FUnitData::ClearStatusEffects()
{
	statusEffects.Empty();
}

TMap<UnitTypes, FUnitComposition> FUnitData::GetUnitComposition() const
{
	TMap<UnitTypes, FUnitComposition> units;

	units.Add(UnitTypes::Infantry, FUnitComposition{});
	units.Add(UnitTypes::Cavalry, FUnitComposition{});
	units.Add(UnitTypes::Scout, FUnitComposition{});
	units.Add(UnitTypes::Ranged, FUnitComposition{});
	units.Add(UnitTypes::Shielder, FUnitComposition{});
	units.Add(UnitTypes::Settler, FUnitComposition{});

	if (unitType != UnitTypes::Army)
	{
		units[unitType].quantity++;
		units[unitType].compPercent = 1.f;

		return units;
	}

	int totalUnits = 0;

	for (int i = 0; i < savedUnits.Num(); i++)
	{
		++units[savedUnits[i]->unitType].quantity;
		++totalUnits;
	}

	for (auto& unit : units)
	{
		unit.Value.compPercent = (float)unit.Value.quantity / (float)totalUnits;
	}

	return units;
}
UnitTypes FUnitData::GetLargestUnitQuantity() const
{
	if (unitType != UnitTypes::Army) return unitType;

	TMap<UnitTypes, FUnitComposition> composition = GetUnitComposition();

	UnitTypes highestType = UnitTypes::None;
	int best = 0;

	for (auto& unit : composition)
	{
		if (unit.Value.quantity > best)
		{
			best = unit.Value.quantity;
			highestType = unit.Key;
		}
	}

	return highestType;
}