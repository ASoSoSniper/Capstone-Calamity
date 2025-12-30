// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementAI.h"
#include "GlobalSpawner.h"
#include "AITroopComponent.h"
#include "UAI_Controller.h"
#include "HexNav.h"
#include "Troop.generated.h"

/**
 * 
 */
class AMergedArmy;
class UAITroopComponent;

UCLASS()
class CAPSTONEPROJECT_API ATroop : public AMovementAI, public IUAI_Controller
{
	GENERATED_BODY()
	
public:
	ATroop();
	UPROPERTY(EditAnywhere) UAITroopComponent* AITroopComponent;
	AActor* targetToMerge;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void InitTroop(const Factions& faction, const UnitTypes& unitType);
	void InitTroop(FUnitData* data);
	
	virtual void MergeOnTile();
	
	virtual void Action1();
	virtual void Action2();
	virtual void Action3();
	virtual void Action4();

	UFUNCTION(BlueprintCallable) FTroopUIData GetUIData();
	bool CommandTroopToMerge(AActor* target);
	int GetArmyCap();

	float GetRemainingHexTraversalTime() const;

protected:
	virtual void MoveToTarget(float& DeltaTime) override;
	virtual void CreatePath() override;

	virtual bool DestinationReached() const override;

	virtual void Destroyed() override;

private:
	bool SetUpTroop();
	bool setupComplete;
	void RotateToFaceTarget(FVector direction, float& DeltaTime);

	UPROPERTY(EditAnywhere) float rotateSpeedMultiplier = 3.f;
	UPROPERTY(EditAnywhere) bool EndForVSlice = false;
	UPROPERTY(EditAnywhere) int armyCap = 20;
	UPROPERTY(VisibleAnywhere) bool merging;
	UPROPERTY(EditAnywhere, Category = "Debug") Factions preAssignedFaction = Factions::None;
	UPROPERTY(EditAnywhere, Category = "Debug") UnitTypes preAssignedUnitType = UnitTypes::None;
	UPROPERTY(EditAnywhere, Category = "Debug") bool debug = false;

#pragma region Utility AI
public:
	void AI_SetMovementAction(UAI_Action* action, const ABaseHex* target);
	void AI_SetMovementAction(UAI_Action* action, UHexNav* target);
	void UpdateDestination();
	virtual void EndAction() override;
	UHexNav* GetTargetUnit() const;
private:
	UHexNav* targetUnit;
#pragma endregion
};
