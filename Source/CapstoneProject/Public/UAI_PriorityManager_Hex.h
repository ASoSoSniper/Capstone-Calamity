// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseHex.h"
#include "Faction.h"
#include "UnitActions.h"
#include "StratResources.h"
#include "UAI_HexCondition.h"
#include "UAI_PriorityManager_Hex.generated.h"

USTRUCT(BlueprintType)
struct FHexSearchConditions
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere) TArray<UAI_HexCondition*> conditions;
};

UCLASS(BlueprintType)
class CAPSTONEPROJECT_API UUAI_PriorityManager_Hex : public UActorComponent
{
	GENERATED_BODY()

public:
	UUAI_PriorityManager_Hex();
	void Initialize(UFaction* faction);

	UFUNCTION(BlueprintCallable, BlueprintPure) ABaseHex* GetPriorityHex_Building(SpawnableBuildings building) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ABaseHex* GetPriorityHex_Workers(EStratResources resource) const;

	UFUNCTION() void BindHexDelegates(ABaseHex* hex, bool enable);
	UFUNCTION() void BindBuildingDelegates(ABuilding* building, bool enable);
private:
	UFaction* factionParent;
	TMap<SpawnableBuildings, ABaseHex*> priorityHexes_Building;
	TMap<EStratResources, ABaseHex*> priorityHexes_Workers;
	const TMap<TerrainType, FHexSet>* factionHexes;

	UPROPERTY(EditAnywhere, Category = "Search Conditions") TMap<SpawnableBuildings, FHexSearchConditions> buildingSearchConditions;

	UFUNCTION() void FindPriorityHex_Building(SpawnableBuildings building);
	UFUNCTION() void FindPriorityHex_Workers(EStratResources resource);

	UFUNCTION() void FindPriorityHex_Buildings_All();
	UFUNCTION() void FindPriorityHex_Workers_All();

	UFUNCTION() void HandleOnBuildingSet(ABaseHex* hex);
	UFUNCTION() void HandleOnWorkersSet(ABaseHex* hex);
	UFUNCTION() void HandleOnHexesClaimed();

	float ScoreHex(SpawnableBuildings building, ABaseHex* hex);
};
