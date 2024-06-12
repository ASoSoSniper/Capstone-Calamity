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
#include "CapitalHub.h"
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

	//Selection Order
	UPROPERTY(VisibleAnywhere, Category = "Selection Cycling") bool selectionCycle;
	//bool toggleBuildingUI = false;
	//UFUNCTION(Blueprintable) bool ToggleBuildingUI();

	//Sound Things
	UPROPERTY(VisibleAnywhere) APlayerMovement* playerCamera;
	UPROPERTY(EditAnywhere, Category = "Troop Sounds") TMap<UnitTypes, USoundBase*> UITroopSounds;
	UPROPERTY(EditAnywhere, Category = "Troop Sounds") TMap<Factions, USoundBase*> UITroopSelectionSounds;
	UPROPERTY(EditAnywhere, Category = "Troop Sounds") USoundBase* troopCompleteSound;
	UPROPERTY(EditAnywhere, Category = "Miscellaneous Sounds") USoundBase* selectFailSound;
	UPROPERTY(EditAnywhere, Category = "Building Sounds") USoundBase* buildingCompleteSound;
	UPROPERTY(EditAnywhere, Category = "Building Sounds") TMap<SpawnableBuildings, USoundBase*> UIBuildingSounds;
	UPROPERTY(EditAnywhere, Category = "Building Sounds") TMap<BuildingAttachments, USoundBase*> UIAttachmentSounds;
	UPROPERTY(EditAnywhere, Category = "Hex Sounds") TMap<TerrainType, USoundBase*> UIHexSounds;
	UFUNCTION(BlueprintCallable) void PlayUITroopSound(UnitTypes unitType);
	UFUNCTION(BlueprintCallable) void PlayUITroopSelectionSound(Factions faction);
	UFUNCTION(BlueprintCallable) void PlayUIBuildingSound(SpawnableBuildings buildingType);
	UFUNCTION(BlueprintCallable) void PlayUIAttachmentSound(BuildingAttachments attachmentType);
	UFUNCTION(BlueprintCallable) void PlayUIHexSound(TerrainType hexType, ABaseHex* hex = nullptr);

	UPROPERTY(EditAnywhere, Category = "Battle Sounds") USoundBase* battleStartSound;
	UPROPERTY(EditAnywhere, Category = "Battle Sounds") USoundBase* battleVictorySound;
	UPROPERTY(EditAnywhere, Category = "Battle Sounds") USoundBase* battleDefeatSound;

	void SetCinematicObject(AActor* object, GameStates gameState = GameStates::None);

	//Interactable object the cursor hovers over
	UPROPERTY(VisibleAnywhere) AActor* hoveredWorldObject;

	//Interactable object the player has clicked to select
	UPROPERTY(VisibleAnywhere) AActor* selectedWorldObject;

	//Active when left click is held down, with the intent to select
	UPROPERTY(VisibleAnywhere) bool activeSelecting;
	
	//Additional info on selectedWorldObject if it is identified as a movable unit
	UPROPERTY(VisibleAnywhere) ABaseHex* selectedHex;

	UPROPERTY(VisibleAnywhere) ActionStates currentActionState = ActionStates::None;
	TMap<ActionStates, UManageMode*> actionStates;

	UPROPERTY(BlueprintReadWrite) Factions playerFaction = Factions::Human;

	void SetHoveredWorldObject(AActor* object);
	void SetSelectedWorldObject(AActor* object);
	void SetActionState();
	UFUNCTION(BlueprintCallable) AActor* GetActionStateSelection();
	UFUNCTION(BlueprintCallable) void Deselect();
	UFUNCTION(BlueprintCallable) void CommandAction();
	UFUNCTION(BlueprintCallable) ActionStates GetActionState();
	UFUNCTION(BlueprintCallable) void ForceActionState(int state);
	UFUNCTION(BlueprintCallable) bool IsActionState(ActionStates state);

	void Build(SpawnableBuildings building);

	UFUNCTION(BlueprintCallable) TArray<int> GetPlayerResources();
	UFUNCTION(BlueprintCallable) void SetPlayerResources(int foodCost, int prodCost, int energyCost, int wealthCost, int popCost, bool overrideCosts = true);
	UFUNCTION(BlueprintCallable) int GetPlayerPopulation();

	UPROPERTY(VisibleAnywhere) AGlobalSpawner* spawner;

	UFUNCTION(BlueprintCallable) FWorkersInHex GetWorkersInHex();
	UFUNCTION(BlueprintCallable) FWorkersInHex GetWorkersInAttachment(BuildingAttachments attachment);
	UFUNCTION(BlueprintCallable) TArray<FBuildingDisplay> GetBuildingDisplays();
	UPROPERTY(VisibleAnywhere) bool firstBuildPerformed = false;
	UFUNCTION(BlueprintCallable) FBuildingDisplay GetBuildingDisplay(ABuilding* building);
	UFUNCTION(BlueprintCallable) TArray<FTroopDisplay> GetTroopDisplays();
	UFUNCTION(BlueprintCallable) TArray<FBuildingDisplay> GetAttachmentDisplays();
	UFUNCTION(BlueprintCallable) FBuildingDisplay GetAttachmentDisplay(FText attachmentName);

	UFUNCTION(BlueprintCallable) void EnterSelectionMode(bool active);
	UFUNCTION(BlueprintCallable) bool IsInBuildMode();
	UFUNCTION(BlueprintCallable) void SelectBuilding(FText buildingName);
	UFUNCTION(BlueprintCallable) void DestroyBuilding();
	UFUNCTION(BlueprintCallable) void RetreatFromBattle();
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
	UFUNCTION(BlueprintCallable) FWorkerSliders SetAttachmentWorkerCount(FWorkerSliders sliders, BuildingAttachments attachmentType);

	AOutpost* GetOutpost();
	UFUNCTION(BlueprintCallable) FBuildingOnHex GetBuildingOnHex();

	UFUNCTION(BlueprintCallable) bool OutpostCanBuildTroops();
	UFUNCTION(BlueprintCallable) bool OutpostCanStoreTroops();
	UFUNCTION(BlueprintCallable) bool AttachmentIsActive(FText attachmentName);
	UFUNCTION(BlueprintCallable) bool AttachmentIsBuilt(BuildingAttachments attachment);

	UFUNCTION(BlueprintCallable) bool ToggleFarmlandYield(bool produceFood);
	UFUNCTION(BlueprintCallable) bool FarmlandYieldsFood();

	void CheckForActionStates();

	UFUNCTION(BlueprintCallable) void ResourceCheats(int resourceToChange, int val);
	UFUNCTION(BlueprintCallable) void ChangeArmyName(FString newName);
	UFUNCTION(BlueprintCallable) FBuildingTTInfo GetBuildingTTDisplay(FText buildingName);
	UFUNCTION(BlueprintCallable) FTroopTTInfo GetTroopTTDisplay(FText troopName);

	UFUNCTION(BlueprintCallable) void PlayUISound(USoundBase* sound);

	UFUNCTION(BlueprintCallable) TArray<FTroopArmyDisplay> GetBattleUnits(int group);
	UFUNCTION(BlueprintCallable) FTroopArmyDisplay GetBattleUnit(int group);
	UFUNCTION(BlueprintCallable) FArmyDisplay DisplaySelectedUnit();
	UFUNCTION(BlueprintCallable) FArmyMenuInfo DisplayArmyMenu();
	UFUNCTION(BlueprintCallable) void SplitArmyInHalf();
	UFUNCTION(BlueprintCallable) void ExtractUnitFromArmy(UnitTypes unitType);

	UFUNCTION(BlueprintCallable) FAttachmentBuildProgress GetAttachmentBuildProgress(BuildingAttachments attachment);
	UFUNCTION(BlueprintCallable) FAttachmentTTBuildInfo GetAttachmentBuildInfo(BuildingAttachments attachment);

	UFUNCTION(BlueprintCallable) FMaterialStorageStats GetMaterialStorageStats();
	UFUNCTION(BlueprintCallable) FRobotFactoryStats GetRobotFactoryStats();
	UFUNCTION(BlueprintCallable) FRobotStorageStats GetRobotStorageStats();
	UFUNCTION(BlueprintCallable) FDefenseStationStats GetDefenseStationStats();

	UFUNCTION(BlueprintCallable) bool CanBuildOnHex();
	UFUNCTION(BlueprintCallable) bool CanPutWorkersOnHex();
	UFUNCTION(BlueprintCallable) bool HexIsDiscovered();


	UFUNCTION(BlueprintCallable) bool IsHumanControlled(AActor* object);
	UFUNCTION(BlueprintCallable) bool HexIsHumanControlled();

	UFUNCTION(BlueprintCallable) bool BattleIsSiege();
	UFUNCTION(BlueprintCallable) FSiegeBuildingInfo GetSiegeBuildingInfo();

	UFUNCTION(BlueprintCallable) bool UnitTypeOnHex(UnitTypes type);
	UFUNCTION(BlueprintCallable) bool BattleGroupIsHuman(int group);
	UFUNCTION(BlueprintCallable) FPlayerWorkers GetPlayerWorkers();
};
