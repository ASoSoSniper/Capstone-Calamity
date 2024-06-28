// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "BaseHex.h"
#include "GlobalSpawner.h"
#include "Outpost.generated.h"

class UOutpostDefenses;
class UOutpostStorage;
class UOutpostBarracks;
class UOutpostTroopFactory;
class UBuildingAttachment;

/**
 * 
 */

UCLASS()
class CAPSTONEPROJECT_API AOutpost : public ABuilding
{
	GENERATED_BODY()

public:

	AOutpost();

	virtual void Tick(float DeltaTime) override;

	virtual UBuildingAttachment* GetAttachment(BuildingAttachments attachment);
	void BuildAttachment(BuildingAttachments attachment);
	bool BuildingAttachmentIsActive(BuildingAttachments attachment);

	void CueTroopBuild(UnitTypes unit);
	float GetTroopBuildTime();
	UnitTypes GetCuedTroop();
	
	void StoreTroop(ATroop* troop);

	virtual void Action1() override;
	virtual void Action2() override;
	virtual void Action3() override;
	virtual void Action4() override;

	virtual void Destroyed() override;
protected:

	UPROPERTY(EditAnywhere) UOutpostStorage* storageBuilding;
	UPROPERTY(EditAnywhere) UOutpostBarracks* barracksBuilding;
	UPROPERTY(EditAnywhere) UOutpostTroopFactory* troopFactoryBuilding;
	UPROPERTY(EditAnywhere) UOutpostDefenses* defenseBuilding;

	UPROPERTY(EditAnywhere) int range = 2;

	UPROPERTY(EditAnywhere)
		float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere)
		float traceLength = 50.f;
	UPROPERTY(VisibleAnywhere) TArray<ABaseHex*> claimedHexes;

private:
	TArray<UnitActions::UnitData> troopsInStorage;

	TArray<ABaseHex*> ClaimLand();

	TArray<ABaseHex*> ScanHex(ABaseHex* hex);

	void BuildingAction() override;
	void BuildTroop();
	virtual void SetToFinishedModel();
	void AddWorkersToAttachment(BuildingAttachments attachment, WorkerType worker, int value);

	TArray<UnitTypes> cuedUnits;
	float currentTroopBuildTime;

	void HealTroops(float& DeltaTime);
	UPROPERTY(EditAnywhere) float healRate = 1.f;
	float currentHealRate = 0;
	UPROPERTY(EditAnywhere) int healAmount = 5;

	TArray<ATroop*> ReleaseTroops();
};
