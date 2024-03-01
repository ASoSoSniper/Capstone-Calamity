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

	UPROPERTY(EditAnywhere) int range = 4;
	TArray<ABaseHex*> ClaimLand();

	TArray<ABaseHex*> ScanHex(ABaseHex* hex);
	
	UPROPERTY(EditAnywhere)
		float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere)
		float traceLength = 50.f;
	UPROPERTY(VisibleAnywhere) TArray<ABaseHex*> claimedHexes;
	
	void BuildingAction() override;

	virtual UBuildingAttachment* GetAttachment(BuildingAttachments attachment);
	void BuildAttachment(BuildingAttachments attachment);
	void AddWorkersToAttachment(BuildingAttachments attachment, WorkerType worker, int value);
	bool BuildingAttachmentIsActive(BuildingAttachments attachment);

	UPROPERTY(EditAnywhere) UOutpostStorage* storageBuilding;
	UPROPERTY(EditAnywhere) UOutpostBarracks* barracksBuilding;
	UPROPERTY(EditAnywhere) UOutpostTroopFactory* troopFactoryBuilding;
	UPROPERTY(EditAnywhere) UOutpostDefenses* defenseBuilding;

	TArray<SpawnableUnits> cuedUnits;
	float currentTroopBuildTime;
	void CueTroopBuild(SpawnableUnits unit);
	
	TArray<UnitActions::UnitData> troopsInStorage;
	void StoreTroop(ATroop* troop);
	TArray<ATroop*> ReleaseTroops();

	virtual void Action1() override;
	virtual void Action2() override;
	virtual void Action3() override;
	virtual void Action4() override;

	virtual void Destroyed() override;
};
