// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "HexNav.h"
#include "GlobalSpawner.h"
#include "UnitActions.h"
#include "Components/AudioComponent.h"
#include "BattleObject.generated.h"

UCLASS(BlueprintType)
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

	UPROPERTY(EditAnywhere) UInteractable* interact;
	UPROPERTY(EditAnywhere) UHexNav* hexNav;
	UPROPERTY(EditAnywhere) UMeshVisibility* visibility;
	ABaseHex* hex;

	UPROPERTY(EditAnywhere) USkeletalMeshComponent* group1Mesh;
	UPROPERTY(EditAnywhere) USkeletalMeshComponent* group2Mesh;
	UPROPERTY(EditAnywhere) USceneComponent* baseRoot;

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

	TArray<TMap<UnitTypes, FUnitComposition>> groupCompositions;

	virtual void Start();

	void CreateFactions();
	void AddUnitToFaction(AMovementAI* troop);
	void GenerateArmies();
	virtual void GenerateModels();
	void AddUnitToArmy(UnitActions::UnitData data);

	void AssignGroups();

	virtual void Attack();
	virtual void EndBattle();
	virtual void DestroyBattle();
	bool IsAlive(UnitActions::UnitData& group);
	Factions FleeFromBattle(Factions faction);
	TArray<ATroop*> ExtractFactionUnits(Factions faction, bool spawnAtOutpost = false);
	void RemoveArmy(Factions faction);

	UPROPERTY(EditAnywhere) float attackRate = 2.f;
	float currentAttackTime;
	bool attacking;
	bool ending;
	UPROPERTY(EditAnywhere) float timeTillEnd = 5.f;

	AGlobalSpawner* spawner;

	UPROPERTY(VisibleAnywhere) int group1Die = 0;
	UPROPERTY(VisibleAnywhere) int group2Die = 0;
	UPROPERTY(VisibleAnywhere) int group1Damage = 0;
	UPROPERTY(VisibleAnywhere) int group2Damage = 0;

	UPROPERTY(EditAnywhere) int ticksTillRoll = 5;
	int currentTickTillRoll = 0;

	int RollDie(int& groupDie);
	float GetRollModifier(int& groupDie);

	UPROPERTY(EditAnywhere) float moraleDecayRate = 0.01f;
	float GetMoralePercent(Factions faction);
	float DecayMorale(Factions faction, float percentReduction);

	Factions attackingFaction = Factions::None;
	void CalculateGroupDamage();
	TMap<UnitTypes, FUnitComposition> GetArmyComposition(TArray<Factions>& group);
};
