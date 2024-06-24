// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitActions.h"
#include "AITroopComponent.generated.h"

class ATroop;
class ABaseHex;
class ABuilding;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UAITroopComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAITroopComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere) int troopsInArmy = 1;

	void EnableEnemyAI();

private:
	enum MoveTarget
	{
		ToHex,
		ToEnemy,
		HoldPosition
	};
	MoveTarget currentState;

	UPROPERTY(VisibleAnywhere) ATroop* parentTroop = nullptr;

	ABaseHex* FindHex(int X, int Y);

	AActor* SelectClosestHostileTarget(ObjectTypes targetType = ObjectTypes::NoType);
	bool IsViableTarget(ABaseHex* hex, ObjectTypes targetType = ObjectTypes::NoType);
	AActor* FindRandomHex();

	UPROPERTY(VisibleAnywhere) bool isEnemy = false;

	UPROPERTY(EditAnywhere) int randomHexInterval = 5;
	UPROPERTY(EditAnywhere) int targetAttackDistance = 15;

	void GenerateArmy();
	void UpdateBehavior();

	bool CanFindOccupiableBuilding();
	bool CanFindEnemyTarget();
	void SetNeutralDestination();
	void SetHostileDestination(AActor* hex);
	bool OccupyingBuilding();
};
