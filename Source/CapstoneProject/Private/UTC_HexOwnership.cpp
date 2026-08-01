// Fill out your copyright notice in the Description page of Project Settings.


#include "UTC_HexOwnership.h"
#include "Faction.h"

float UUTC_HexOwnership::ScoreCondition(ATroop* troop, ABaseHex* hex) const
{
	if (!troop || !hex) return FactorInversion(0.f);

	UFaction* troopFaction = troop->GetFaction();
	EFactions hexFaction = hex->GetHexOwner();

	if (!troopFaction) return FactorInversion(0.f);

	EFactionRelationship relationship = troopFaction->GetFactionRelationship(hexFaction);

	return FactorInversion(relationship == hexRelationship);
}
