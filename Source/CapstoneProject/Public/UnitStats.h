// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FactionEnum.h"
#include "UnitActions.h"
#include "UnitStats.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UUnitStats : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitStats();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsAlive();
	void Heal(int amount);
	void RecoverMorale();

	UPROPERTY(EditAnywhere) Factions faction;
	UPROPERTY(EditAnywhere) UnitTypes unitType;
	UPROPERTY() FString name;
	UPROPERTY() int32 nameInstance;
	FString GetUnitName();
	UPROPERTY(VisibleAnywhere) bool upgraded;
	//Exclusive property for armies
	//TArray<FUnitData> savedUnits;

	//Health and Morale
	UPROPERTY(EditAnywhere) int currentHP = 10;
	UPROPERTY(EditAnywhere) int maxHP = 10;
	UPROPERTY(EditAnywhere) int currentMorale = 5;
	UPROPERTY(EditAnywhere) int maxMorale = 5;

	//Environment
	UPROPERTY(EditAnywhere) int vision = 1;
	UPROPERTY(EditAnywhere) float speed = 2.f;
	
	//Damage
	UPROPERTY(EditAnywhere) int damage = 3;
	UPROPERTY(EditAnywhere) int siegePower = 10;

	//Other
	UPROPERTY(EditAnywhere) int reinforceRate = 2;
	UPROPERTY(EditAnywhere) int energyUpkeep = 2;
	
	float maxHealTime = 5.f;
	float currhealTime = 5.f;

	float moraleRecovery = 0.1f;	
};
