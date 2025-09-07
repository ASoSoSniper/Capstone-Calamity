// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "HexNav.h"
#include "GlobalSpawner.h"
#include "UnitActions.h"
#include "Components/AudioComponent.h"
#include "CombatAnims.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* group1Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* group2Mesh;

	UPROPERTY(EditAnywhere) UCombatAnims* group1Anims;
	UPROPERTY(EditAnywhere) UCombatAnims* group2Anims;

	UPROPERTY(EditAnywhere) USceneComponent* baseRoot;

	//Every troop participating in this battle, sorted into their factions
	TMap<Factions, TArray<FUnitData*>> factionsInBattle;

	//The singular faction units created using the unit data from factionsInBattle
	TMap<Factions, FUnitData*> armies;

	struct Battle
	{
		TArray<Factions> Group1;
		TArray<Factions> Group2;
	};
	Battle currentBattle;

	TArray<TMap<UnitTypes, FUnitComposition>> groupCompositions;

	virtual void Start();

	UFUNCTION(BlueprintCallable) float DisplayBattleProgress();

	bool IsEnding();
	bool IsAttacking();

	void SetSelected(bool selected);
	int GetGroup1Die();
	int GetGroup2Die();
	ABaseHex* GetHex();

	Factions FleeFromBattle(Factions faction);

protected:

	void CreateFactions();
	void AddUnitToFaction(AMovementAI* troop);
	virtual void GenerateModels();
	void AddUnitToArmy(FUnitData* data);
	void AssignFactionToGroup(Factions army);

	TArray<ATroop*> ExtractFactionUnits(Factions faction, bool spawnAtOutpost = false);
	void RemoveArmy(Factions faction);

	virtual void Attack();
	virtual void EndBattle();
	virtual void DestroyBattle();
	virtual void Destroyed() override;

	int RollDie(int& groupDie);
	float GetRollModifier(int& groupDie);

	float DecayMorale(Factions faction, float percentReduction);
	void CalculateGroupDamage();

	UPROPERTY(EditAnywhere) bool attacking;
	bool ending;
	ABaseHex* hex;

	UPROPERTY(VisibleAnywhere) int group1Die = 0;
	UPROPERTY(VisibleAnywhere) int group2Die = 0;
	UPROPERTY(VisibleAnywhere) int group1Damage = 0;
	UPROPERTY(VisibleAnywhere) int group2Damage = 0;

	UPROPERTY(EditAnywhere) int ticksTillRoll = 5;
	int currentTickTillRoll = 0;
	UPROPERTY(EditAnywhere) float moraleDecayRate = 0.01f;

private:
	UPROPERTY(EditAnywhere) float attackRate = 2.f;
	float currentAttackTime;
	
	UPROPERTY(EditAnywhere) float timeTillEnd = 5.f;

	Factions attackingFaction = Factions::None;

	bool selectedByPlayer;

	TMap<UnitTypes, FUnitComposition> GetArmyComposition(TArray<Factions>& group);
};
