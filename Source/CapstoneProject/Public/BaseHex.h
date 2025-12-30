// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Components/SphereComponent.h"
#include "TerrainEnum.h"
#include "StratResources.h"
#include "UnitActions.h"
#include "MeshVisibility.h"
#include "BaseHex.generated.h"

class AMergedArmy;
class AMovementAI;
class ABuilding;
class AGlobalSpawner;
class AInvestigator;

USTRUCT(BlueprintType)
struct FHexInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int food;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int production;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int defenderBonus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float moveMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float attritionMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int visionModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
};

USTRUCT(BlueprintType)
struct FHexDisplay
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText food;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText production;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText energy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText wealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText workerCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText defenderBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText moveMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText attritionBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText visionModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* icon;
};

USTRUCT(BlueprintType)
struct FCurrentResourceYields
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int foodYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int energyYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int productionYield;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int wealthYield;
};

UENUM(BlueprintType)
enum class EHexSearchRules : uint8
{
	ContainsAny,
	ContainsAllies,
	ContainsEnemies
};

UCLASS()
class CAPSTONEPROJECT_API ABaseHex : public AActor
{
	GENERATED_BODY()
	
#pragma region General Logic
public:	
	ABaseHex();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* troopAnchor;
	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* buildingAnchor;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Components") UInteractable* interactable;
#pragma endregion

#pragma region Identity
public:
	Factions GetHexOwner();
	void SetHexOwner(Factions faction);

	FVector2D GetHexCoordinates() const;
	void SetHexCoordinates(int x, int y);

	ABaseHex* FindFreeAdjacentHex(Factions faction, TSet<ABaseHex*>& usedHexes, bool includeSelf = true, EHexSearchRules occupancyFilter = EHexSearchRules::ContainsAny, bool includeBuildings = false);
	TSet<ABaseHex*> GetHexesInRadius(const int layers = 1, bool includeSelf = true) const;

	TerrainType GetHexTerrain();
	int GetMovementMulti() const;
	int GetAttritionMulti() const;
	int GetDefenderBonus() const;
	int GetVision() const;

	bool IsStaticBuildingTerrain();
	bool IsTraversableTerrain() const;
	bool IsBuildableTerrain() const;
	bool IsPlayerHex();
	bool CanPutWorkersOnHex();

private:
	UPROPERTY(EditAnywhere, Category = "Identity") Factions hexOwner = Factions::None;
	TerrainType hexTerrain = TerrainType::Plains;
	FVector2D hexCoordinates;
	UPROPERTY(EditAnywhere, Category = "Identity") TMap<TerrainType, FHexInfo> hexInfo;
	UPROPERTY(VisibleAnywhere, Category = "Identity") float moveMultiplier = 1.f;
	UPROPERTY(VisibleAnywhere, Category = "Identity") float attritionMultiplier = 1.f;
	UPROPERTY(VisibleAnywhere, Category = "Identity") int defenderBonus = 0;
	UPROPERTY(VisibleAnywhere, Category = "Identity") int vision = 0;
#pragma endregion

#pragma region Workers
public:
	int GetMaxWorkers() const;
	void SetMaxWorkers(int newMax);
	int GetNumberOfWorkers() const;
	bool WorkersAtCapacity() const;

	void UpdateWorkerDisplay();

	TMap<WorkerType, int> workersInHex;
protected:
	UFUNCTION(BlueprintImplementableEvent) void SetWorkerDisplay(const int& current, const int& max);
private:
	UPROPERTY(VisibleAnywhere, Category = "Identity") int maxWorkers = 15;
	UPROPERTY(EditAnywhere, Category = "Identity") int maxWorkersDefault = 5;
#pragma endregion

#pragma region Troops and Buildings
public:
	UFUNCTION(BlueprintCallable, BlueprintPure) AMovementAI* GetFurthestTravelingTroop() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HexContainsTroop(AMovementAI* troop) const;
	void AddTroopToHex(AMovementAI* troop);
	void RemoveTroopFromHex(AMovementAI* troop);
	UPROPERTY(VisibleAnywhere) TArray<AMovementAI*> troopsInHex;

	UFUNCTION(BlueprintCallable, BlueprintPure) bool CanBuildOnHex(int requiredLayers) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ABuilding* GetBuilding() const;
	void AddBuildingToHex(ABuilding* setBuilding, int layers = 0);
	void RemoveBuildingFromHex(int layers = 0);
	UPROPERTY(VisibleAnywhere) ABuilding* building;

	TArray<AActor*> GetObjectsInHex() const;

private:
#pragma endregion

#pragma region Battles
public:
	void BeginBattle(AMovementAI* attacker = nullptr);

	void CheckForHostility(AMovementAI* refTroop);
	void CheckForHostility(ABuilding* refBuilding);
	
	AMovementAI* GetAttackingTroop();
	UFUNCTION(BlueprintCallable, BlueprintPure) ABattleObject* GetBattle() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) bool ActiveBattleOnHex() const;

	UPROPERTY(VisibleAnywhere) ABattleObject* battle;

private:
	AMovementAI* attackingTroop;
#pragma endregion
	
#pragma region Resources and Harvesting
public:
	bool ActiveHarvesting();
	float GetOutputPercent();

	void UpdateResourceYield(EStratResources resource, int value, Factions faction = Factions::None);
	void ToggleResourceYield();
	FCurrentResourceYields GetCurrentResourceYields();
	const TMap<EStratResources, int>& GetHexResources() const;
private:
	TMap<EStratResources, int> resourceBonuses;
	bool harvesting;
	float outputPercent;
#pragma endregion

#pragma region Visuals
public:
	void SetHexTerrain(int maxSeedSize = 5, int randToMaintain = 5);
	void SetHexTerrain(TerrainType terrain);
	void SetHexModel();
	void SetAttachmentCanBeVisible(bool canBeVisible);

	UFUNCTION(BlueprintCallable, BlueprintPure) bool VisibleToPlayer() const;

	FHexDisplay GetDisplayInfo();
	UFUNCTION(BlueprintImplementableEvent) void PrintCoordinates(int x, int y);
	UFUNCTION(BlueprintImplementableEvent) void ToggleUI(bool active);

	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexMeshAttachment;
	UPROPERTY(VisibleAnywhere, Category = "Components") UStaticMeshComponent* hexBase;
	UPROPERTY(VisibleAnywhere, Category = "Components") UMeshVisibility* visibility;
private:
	int seedIndex = 0;

	UPROPERTY(VisibleAnywhere, Category = "Debug") bool attachmentCanBeVisible;
	UPROPERTY(EditAnywhere, Category = "Debug") bool debug = false;
#pragma endregion

#pragma region Sounds
public:
	float GetTargetVolume();
	void SetTargetVolume(float volume);
	void SetInSoundBoxRadius(bool inRadius);

	UPROPERTY(VisibleAnywhere, Category = "Components") UAudioComponent* audioComponent;
private:
	UPROPERTY(EditAnywhere, Category = "Sound") float volumeSpeed = 1.5f;
	UPROPERTY(VisibleAnywhere, Category = "Sound") float targetVolume = 0.f;
	void SetToTargetVolume(float& DeltaTime);
	bool inSoundboxRadius = false;
#pragma endregion

#pragma region Status Effects
public:
	void AddEffectToHex(FStatusEffect* effect);
	void RemoveEffectFromHex(FStatusEffect* effect);

private:
	void AddEffectToUnit(FUnitData* data, FStatusEffect* effect, UFaction* factionObject);

	void AddEffectToAllUnits(FStatusEffect* effect);
	void RemoveEffectFromAllUnits(FStatusEffect* effect);

	void AddAllEffectsToUnit(FUnitData* data);
	void RemoveAllEffectsFromUnit(FUnitData* data);

	TSet<FStatusEffect*> statusEffects;
#pragma endregion

#pragma region Point of Interest
public:
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasPOI() const;
	FPointOfInterest* GetPOI();
	UFUNCTION(BlueprintCallable, BlueprintPure) const TMap<EStratResources, int32>& GetPOIRewards() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) int32 GetPOIDaysToComplete() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) FString GetPOIWorldDisplay() const;

	UFUNCTION(BlueprintCallable) void CreatePointOfInterest(FPointOfInterest& setPOI);
	void BeginInvestigation(AMovementAI* investigator);
	void EndPOIInvestigation();

	UFUNCTION(BlueprintCallable, BlueprintPure) AInvestigator* GetInvestigator() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasPOIInvestigator() const;
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasUnsearchedPOI() const;
private:
	UPROPERTY(EditAnywhere, Category = "Point of Interest") TSubclassOf<AInvestigator> investigatorPrefab;
	FPointOfInterest* pointOfInterest;
	AInvestigator* investigatorObject;
#pragma endregion
};