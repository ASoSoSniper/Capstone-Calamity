// Fill out your copyright notice in the Description page of Project Settings.


#include "Types/StatusEffect.h"

FString FStatusEffect::GetEffectName() const
{
	return effectName;
}
EFactions FStatusEffect::GetOriginFaction() const
{
	return originFaction;
}
EFactionRelationship FStatusEffect::GetFactionsToAffect() const
{
	return factionsToAffect;
}
float FStatusEffect::GetHPMod() const
{
	return hpMod;
}
float FStatusEffect::GetMoraleMod() const
{
	return moraleMod;
}
int FStatusEffect::GetVisionMod() const
{
	return visionMod;
}
float FStatusEffect::GetSpeedMod() const
{
	return speedMod;
}
float FStatusEffect::GetDamageMod() const
{
	return damageMod;
}
float FStatusEffect::GetSiegePowerMod() const
{
	return siegePowerMod;
}