// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FactionEnum.h"
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

	UPROPERTY(EditAnywhere) Factions faction;

	UPROPERTY(EditAnywhere) int HP_current = 10;
	UPROPERTY(EditAnywhere) int HP_max = 10;
	UPROPERTY(EditAnywhere) int minDamage = 1;
	UPROPERTY(EditAnywhere) int maxDamage = 3;
	UPROPERTY(EditAnywhere) float attackRate = 1.f;

	enum HostilityStates
	{
		None,
		Passive,
		Attack,
		Defend
	};
	HostilityStates hostility = Passive;
};
