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
	
#pragma region General Logic
public:	
	ABattleObject();
	virtual void Tick(float DeltaTime) override;

	void SetSelected(bool selected);
	ABaseHex* GetHex();

	UPROPERTY(EditAnywhere) UHexNav* hexNav;
	UPROPERTY(EditAnywhere) UMeshVisibility* visibility;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere) UInteractable* interact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* group1Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* group2Mesh;

	UPROPERTY(EditAnywhere) UCombatAnims* group1Anims;
	UPROPERTY(EditAnywhere) UCombatAnims* group2Anims;

	UPROPERTY(EditAnywhere) USceneComponent* baseRoot;

private:
	ABaseHex* hex;
	
	UPROPERTY() bool selectedByPlayer;
#pragma endregion

#pragma region Setup and Teardown
public:
	virtual void Start();
	void CreateFactions();

	bool IsEnding();

protected:
	virtual void GenerateModels();

	virtual bool EndCondition() const;
	virtual void EndBattle();
	virtual void DestroyBattle();
	virtual void Destroyed() override;

	Factions attackingFaction = Factions::None;

private:
	bool ending = false;
	UPROPERTY(EditAnywhere) float timeTillEnd = 5.f;
#pragma endregion

#pragma region Army Management
public:
	Factions FleeFromBattle(Factions faction);

	void GetGroupHealthAndMorale(int groupIndex, int& HP, int& maxHP, int& morale, int& maxMorale) const;

	//The specific number of each troop the armies are composed of, stored for convenience
	TArray<TMap<UnitTypes, FUnitComposition>> groupCompositions;

	struct Battle
	{
		TMap<Factions, FUnitData*> Group1;
		TMap<Factions, FUnitData*> Group2;

		bool AddUnit(FUnitData* data);
		EngagementSelect DetermineConflictAlignment(Factions& unitFaction);
		bool ContainsFaction(Factions faction) const;

		ATroop* ExtractFaction(Factions& faction, ABaseHex* hex, TSet<ABaseHex*> usedHexes, bool spawnAtOutpost = false);
		TArray<ATroop*> ExtractAllFactions(ABaseHex* hex);
	};
	Battle currentBattle;

protected:
	void AddUnitToFaction(AMovementAI* troop);
	void DestroyArmy(Factions faction);

private:
	TMap<UnitTypes, FUnitComposition> GetArmyComposition(TMap<Factions, FUnitData*>& group);
#pragma endregion

#pragma region Combat
public:	
	UFUNCTION(BlueprintCallable) float DisplayBattleProgress();

	bool IsAttacking();

	int GetGroup1Die();
	int GetGroup2Die();

protected:
	void Attack();
	virtual void CalculateGroupDamage(int& group1Damage, int& group2Damage);
	virtual void ApplyGroupDamage(const int& group1Damage, const int& group2Damage);

	int RollDie(int& groupDie);
	float GetRollModifier(int& groupDie);

	float DecayMorale(FUnitData* faction, float percentReduction);

	virtual bool Group1IsAlive(bool& containsHuman) const;
	virtual bool Group2IsAlive(bool& containsHuman) const;

private:
	UPROPERTY(EditAnywhere) bool attacking;

	UPROPERTY(VisibleAnywhere) int group1Die = 0;
	UPROPERTY(VisibleAnywhere) int group2Die = 0;

	UPROPERTY(EditAnywhere) int ticksTillRoll = 5;
	int currentTickTillRoll = 0;
	UPROPERTY(EditAnywhere) float moraleDecayRate = 0.01f;

	UPROPERTY(EditAnywhere) float attackRate = 2.f;
	float currentAttackTime;
#pragma endregion
};
