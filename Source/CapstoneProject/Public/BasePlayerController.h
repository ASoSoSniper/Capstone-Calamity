// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MovementAI.h"
#include "BaseHex.h"
#include "MiningStation.h"
#include "Farmland.h"
#include "PowerPlant.h"
#include "Outpost.h"
#include "Building.h"
#include "Faction.h"
#include "ManageMode.h"
#include "ManageHex.h"
#include "ManageTroop.h"
#include "ManageBuilding.h"
#include "BuildingAttachment.h"
#include "ManageBattle.h"
#include "GlobalSpawner.h"
#include "PlayerMovement.h"
#include "BasePlayerController.generated.h"


UCLASS()
class CAPSTONEPROJECT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();

protected:

	virtual void BeginPlay() override;
private:

	virtual void Tick(float DeltaTime) override;	

public:	
	UPROPERTY() UManageMode* noneMode;
	UPROPERTY() UManageHex* hexMode;
	UPROPERTY() UManageTroop* troopMode;
	UPROPERTY() UManageBuilding* buildingMode;
	UPROPERTY() UManageBattle* battleMode;

	//Sound Things
	UPROPERTY(VisibleAnywhere) APlayerMovement* playerCamera;
	UPROPERTY(EditAnywhere) TMap<UnitTypes, USoundBase*> UITroopSounds;
	UPROPERTY(EditAnywhere) TMap<SpawnableBuildings, USoundBase*> UIBuildingSounds;
	UPROPERTY(EditAnywhere) TMap<BuildingAttachments, USoundBase*> UIAttachmentSounds;
	UPROPERTY(EditAnywhere) TMap<TerrainType, USoundBase*> UIHexSounds;
	UFUNCTION(BlueprintCallable) void PlayUITroopSound(UnitTypes unitType);
	UFUNCTION(BlueprintCallable) void PlayUIBuildingSound(SpawnableBuildings buildingType);
	UFUNCTION(BlueprintCallable) void PlayUIAttachmentSound(BuildingAttachments attachmentType);
	UFUNCTION(BlueprintCallable) void PlayUIHexSound(TerrainType hexType);

	//Interactable object the cursor hovers over
	UPROPERTY(VisibleAnywhere) AActor* hoveredWorldObject;

	//Interactable object the player has clicked to select
	UPROPERTY(VisibleAnywhere) AActor* selectedWorldObject;
	
	//Additional info on selectedWorldObject if it is identified as a movable unit
	UPROPERTY(VisibleAnywhere) ABaseHex* selectedHex;

	UPROPERTY(VisibleAnywhere) ActionStates currentActionState = ActionStates::None;
	TMap<ActionStates, UManageMode*> actionStates;

	UPROPERTY(BlueprintReadWrite) Factions playerFaction = Factions::Human;

	void SetHoveredWorldObject(AActor* object);
	void SetSelectedWorldObject(AActor* object);
	void SetActionState();
	UFUNCTION(BlueprintCallable) void Deselect();
	UFUNCTION(BlueprintCallable) int GetActionState();
	UFUNCTION(BlueprintCallable) void ForceActionState(int state);

	void Build(SpawnableBuildings building);

	UFUNCTION(BlueprintCallable) TArray<int> GetPlayerResources();
	UFUNCTION(BlueprintCallable) void SetPlayerResources(TArray<int> input, bool overrideCosts);
	UFUNCTION(BlueprintCallable) int GetPlayerPopulation();

	UPROPERTY(VisibleAnywhere) AGlobalSpawner* spawner;

	UFUNCTION(BlueprintCallable) FWorkersInHex GetWorkersInHex();
	UFUNCTION(BlueprintCallable) TArray<FBuildingDisplay> GetBuildingDisplays();
	UFUNCTION(BlueprintCallable) FBuildingDisplay GetBuildingDisplay(ABuilding* building);
	UFUNCTION(BlueprintCallable) TArray<FTroopDisplay> GetTroopDisplays();
	UFUNCTION(BlueprintCallable) TArray<FBuildingDisplay> GetAttachmentDisplays();
	UFUNCTION(BlueprintCallable) FBuildingDisplay GetAttachmentDisplay(FText attachmentName);

	UFUNCTION(BlueprintCallable) void EnterSelectionMode(bool active);
	UFUNCTION(BlueprintCallable) bool IsInBuildMode();
	UFUNCTION(BlueprintCallable) void SelectBuilding(FText buildingName);
	UFUNCTION(BlueprintCallable) void SelectBuildingAttachment(FText buildingName);
	UFUNCTION(BlueprintCallable) void SelectTroop(FText troopName);
	UFUNCTION(BlueprintCallable) FHexDisplay GetHexDisplayInfo();
	UFUNCTION(BlueprintCallable) int GetResourceCap();
	UFUNCTION(BlueprintCallable) FResourcesPerTick GetResourcesPerTick();
	UFUNCTION(BlueprintCallable) bool HexHasBuilding();
	UFUNCTION(BlueprintCallable) FCuedTroop GetCuedTroop();
	UFUNCTION(BlueprintCallable) int GetStarveLevel();
	UFUNCTION(BlueprintCallable) int GetPowerOutageLevel();

	UFUNCTION(BlueprintCallable) FResourceGainLoss GetResourceRates();

	UFUNCTION(BlueprintCallable) FWorkerSliders SetWorkerCount(FWorkerSliders sliders);
	UFUNCTION(BlueprintCallable) FWorkerSliders SetAttachmentWorkerCount(FWorkerSliders sliders, FText attachmentName);

	AOutpost* GetOutpost();
	UFUNCTION(BlueprintCallable) FBuildingOnHex GetBuildingOnHex();

	UFUNCTION(BlueprintCallable) bool OutpostCanBuildTroops();
	UFUNCTION(BlueprintCallable) bool OutpostCanStoreTroops();
	UFUNCTION(BlueprintCallable) bool AttachmentIsActive(FText attachmentName);

	UFUNCTION(BlueprintCallable) bool ToggleFarmlandYield(bool produceFood);

	void CheckForActionStates();

	UFUNCTION(BlueprintCallable) void ResourceCheats(int resourceToChange, int val);
	UFUNCTION(BlueprintCallable) FBuildingTTInfo GetBuildingTTDisplay(FText buildingName);
	UFUNCTION(BlueprintCallable) FTroopTTInfo GetTroopTTDisplay(FText troopName);

	UFUNCTION(BlueprintCallable) void PlayUISound(USoundBase* sound);

	UFUNCTION(BlueprintCallable) TArray<FTroopArmyDisplay> GetBattleUnits(int group);
};
