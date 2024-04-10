// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AITroopComponent.generated.h"

class ATroop;
class ABaseHex;
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

	UPROPERTY(VisibleAnywhere) ATroop* parentTroop = nullptr;

	enum MoveTarget
	{
		ToHex,
		ToEnemy
	};
	MoveTarget currentTarget;

	void SetDestination();

	ABaseHex* FindHex(int X, int Y);

	AActor* SelectClosestHostileTarget();
	AActor* FindRandomHex();

	UPROPERTY(VisibleAnywhere) bool isEnemy = false;

	UPROPERTY(EditAnywhere) int randomHexInterval = 5;
	UPROPERTY(EditAnywhere) int targetAttackDistance = 15;
	UPROPERTY(EditAnywhere) int troopsInArmy = 20;

	void GenerateArmy();
};
