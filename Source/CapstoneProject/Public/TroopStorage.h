// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "TroopStorage.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ATroopStorage : public ABuilding
{
	GENERATED_BODY()
	
public:
	ATroopStorage();
	virtual void Tick(float DeltaTime) override;

	void StoreTroop(ATroop* troop);

	TArray<ATroop*> ReleaseAllTroops();
	ATroop* ReleaseOneTroop(int index);

	const UnitActions::UnitData* GetStoredTroopInfo(int index) const;

private:
	TArray<UnitActions::UnitData> troopsInStorage;
	ATroop* ReleaseTroop(int index, ABaseHex* hex, TSet<ABaseHex*>& usedHexes);

	void HealTroops(float& DeltaTime);
	UPROPERTY(EditAnywhere) float healRate = 1.f;
	float currentHealRate = 0;
	UPROPERTY(EditAnywhere) int healAmount = 5;
};
