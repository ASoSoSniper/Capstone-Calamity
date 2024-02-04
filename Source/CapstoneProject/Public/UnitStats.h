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
	void Heal();

	UPROPERTY(EditAnywhere) Factions faction;
	UPROPERTY(VisibleAnywhere) UnitTypes type;
	UPROPERTY(VisibleAnywhere) bool upgraded;
	//Exclusive property for armies
	TArray<UnitActions::UnitData> savedUnits;

	UPROPERTY(EditAnywhere) int HP_current = 10;
	UPROPERTY(EditAnywhere) int HP_max = 10;
	UPROPERTY(EditAnywhere) int defense = 1;

	UPROPERTY(EditAnywhere) float speed = 2.f;
	UPROPERTY(EditAnywhere) int currentMorale = 5;
	UPROPERTY(EditAnywhere) int maxMorale = 5;
	UPROPERTY(EditAnywhere) int minDamage = 1;
	UPROPERTY(EditAnywhere) int maxDamage = 3;
	UPROPERTY(EditAnywhere) int reinforceRate = 2;
	UPROPERTY(EditAnywhere) int energyUpkeepCost = 2;
	
	float maxHealTime = 5.f;
	float currhealTime = 5.f;
};
