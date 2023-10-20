// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "HexNav.h"
#include "UnitActions.h"
#include "BattleObject.generated.h"

UCLASS()
class CAPSTONEPROJECT_API ABattleObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UInteractable* interact;
	UHexNav* hexNav;
	USphereComponent* sphere;
	ABaseHex* hex;

	//Every troop participating in this battle, sorted into their factions
	TMap<Factions, TArray<UnitActions::UnitData>> factionsInBattle;

	//The singular faction units created using the unit data from factionsInBattle
	TMap<Factions, UnitActions::UnitData> armies;

	struct Battle
	{
		TArray<Factions> Group1;
		TArray<Factions> Group2;
	};
	Battle currentBattle;

	void Start();

	void CreateFactions();
	void AddUnitToFaction(AMovementAI* troop);
	void GenerateArmies();
	void AddUnitToArmy(UnitActions::UnitData data);

	void AssignGroups();

	void Attack();
	void EndBattle();
	bool IsAlive(UnitActions::UnitData& group);

	UPROPERTY(EditAnywhere) float attackRate = 2.f;
	float currentAttackTime;
	bool attacking;

	AGlobalSpawner* spawner;
};
