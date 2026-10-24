// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionEnum.h"
#include "StatusEffect.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FStatusEffect
{
	GENERATED_USTRUCT_BODY()

public:
	FStatusEffect() {};
	FStatusEffect(FString name, EFactions faction, EFactionRelationship affected, float hp, float morale, int vision, float speed, float damage, float siegePower) :
		effectName(name), originFaction(faction), factionsToAffect(affected),
		hpMod(hp), moraleMod(morale), visionMod(vision), speedMod(speed), damageMod(damage), siegePowerMod(siegePower) {
	};

	FString GetEffectName() const;
	EFactions GetOriginFaction() const;
	EFactionRelationship GetFactionsToAffect() const;

	float GetHPMod() const;
	float GetMoraleMod() const;
	int GetVisionMod() const;
	float GetSpeedMod() const;
	float GetDamageMod() const;
	float GetSiegePowerMod() const;

private:
	FString effectName = TEXT("");
	EFactions originFaction = EFactions::None;
	EFactionRelationship factionsToAffect = EFactionRelationship::Neutral;

	float hpMod = 1.f;
	float moraleMod = 1.f;

	int visionMod = 0;
	float speedMod = 1.f;

	float damageMod = 1.f;
	float siegePowerMod = 1.f;
};