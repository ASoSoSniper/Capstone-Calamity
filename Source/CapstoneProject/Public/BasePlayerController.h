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
#include "stdlib.h"
#include "BasePlayerController.generated.h"


UCLASS()
class CAPSTONEPROJECT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region General Logic
public:
	ABasePlayerController();

protected:
	virtual void BeginPlay() override;

private:
	virtual void Tick(float DeltaTime) override;	
#pragma endregion

#pragma region Selection and Action States
public:	
	UPROPERTY(VisibleAnywhere, Category = "Selection Cycling") bool selectionCycle;
	//Interactable object the cursor hovers over
	UPROPERTY(VisibleAnywhere) AActor* hoveredWorldObject;
	//Interactable object the player has clicked to select
	UPROPERTY(VisibleAnywhere) AActor* selectedWorldObject;
	//Additional info on selectedWorldObject if it is identified as a movable unit
	UPROPERTY(VisibleAnywhere) ABaseHex* selectedHex;

	//Active when left click is held down, with the intent to select
	UPROPERTY(VisibleAnywhere) bool activeSelecting;

	UPROPERTY(VisibleAnywhere) ActionStates currentActionState = ActionStates::None;
	TMap<ActionStates, UManageMode*> actionStates;
	UPROPERTY(BlueprintReadWrite) Factions playerFaction = Factions::Human;

	void SetHoveredWorldObject(AActor* object);
	void SetSelectedWorldObject(AActor* object);
	
	UFUNCTION(BlueprintCallable) AActor* GetActionStateSelection();

	UFUNCTION(BlueprintCallable) void Deselect();
	UFUNCTION(BlueprintCallable) void CommandAction();
	UFUNCTION(BlueprintCallable) ActionStates GetActionState();
	UFUNCTION(BlueprintCallable) void ForceActionState(int state);
	UFUNCTION(BlueprintCallable) bool IsActionState(ActionStates state);

	UFUNCTION(BlueprintCallable) void EnterSelectionMode(bool active);
	UFUNCTION(BlueprintCallable) bool IsInBuildMode();

private:
	UPROPERTY() UManageMode* noneMode;
	UPROPERTY() UManageHex* hexMode;
	UPROPERTY() UManageTroop* troopMode;
	UPROPERTY() UManageBuilding* buildingMode;
	UPROPERTY() UManageBattle* battleMode;

	void SetActionState();
#pragma endregion

#pragma region Cinematics and Sounds
public:
	UPROPERTY(EditAnywhere, Category = "Troop Sounds") USoundBase* troopCompleteSound;
	UPROPERTY(EditAnywhere, Category = "Miscellaneous Sounds") USoundBase* selectFailSound;
	UPROPERTY(EditAnywhere, Category = "Building Sounds") USoundBase* buildingCompleteSound;

	UPROPERTY(EditAnywhere, Category = "Battle Sounds") USoundBase* battleStartSound;
	UPROPERTY(EditAnywhere, Category = "Battle Sounds") USoundBase* battleVictorySound;
	UPROPERTY(EditAnywhere, Category = "Battle Sounds") USoundBase* battleDefeatSound;
	
	UFUNCTION(BlueprintCallable) void PlayUISound(USoundBase* sound);
	UFUNCTION(BlueprintCallable) void PlayUITroopSound(UnitTypes unitType);
	UFUNCTION(BlueprintCallable) void PlayUITroopSelectionSound(Factions faction);
	UFUNCTION(BlueprintCallable) void PlayUIBuildingSound(SpawnableBuildings buildingType);
	UFUNCTION(BlueprintCallable) void PlayUIAttachmentSound(BuildingAttachments attachmentType);
	UFUNCTION(BlueprintCallable) void PlayUIHexSound(TerrainType hexType, ABaseHex* hex = nullptr);

	void SetCinematicObject(AActor* object, GameStates gameState = GameStates::None);

private:
	UPROPERTY(VisibleAnywhere) APlayerMovement* playerCamera;
	UPROPERTY(EditAnywhere, Category = "Troop Sounds") TMap<UnitTypes, USoundBase*> UITroopSounds;
	UPROPERTY(EditAnywhere, Category = "Troop Sounds") TMap<Factions, USoundBase*> UITroopSelectionSounds;
	UPROPERTY(EditAnywhere, Category = "Building Sounds") TMap<SpawnableBuildings, USoundBase*> UIBuildingSounds;
	UPROPERTY(EditAnywhere, Category = "Building Sounds") TMap<BuildingAttachments, USoundBase*> UIAttachmentSounds;
	UPROPERTY(EditAnywhere, Category = "Hex Sounds") TMap<TerrainType, USoundBase*> UIHexSounds;
#pragma endregion

#pragma region Building Commands
public:
	UFUNCTION(BlueprintCallable) void SelectBuilding(SpawnableBuildings buildingName);
	UFUNCTION(BlueprintCallable) void DestroyBuilding();
	UFUNCTION(BlueprintCallable) FBuildingOnHex GetBuildingOnHex();

	UFUNCTION(BlueprintCallable) FBuildingDisplay GetBuildingDisplay(SpawnableBuildings building);
	UFUNCTION(BlueprintCallable) TArray<FBuildingDisplay> GetBuildingDisplays();

	UFUNCTION(BlueprintCallable) FBuildingTTInfo GetBuildingTTDisplay(SpawnableBuildings buildingName);

	UFUNCTION(BlueprintCallable) bool ToggleFarmlandYield(bool produceFood);
	UFUNCTION(BlueprintCallable) bool FarmlandYieldsFood();
	UFUNCTION(BlueprintCallable) FCuedTroop GetCuedTroop();

	UFUNCTION(BlueprintCallable) void ReleaseStoredTroop(int index);
	UFUNCTION(BlueprintCallable) void ReleaseAllStoredTroops();
	UFUNCTION(BlueprintCallable) TArray<FTroopUIData> GetAllStoredTroops(ATroopStorage* setStorage = nullptr);

private:
	UPROPERTY() bool firstBuildPerformed = false;
#pragma endregion

#pragma region Attachment Commands
public:
	UFUNCTION(BlueprintCallable) void SelectBuildingAttachment(FText buildingName);
	UFUNCTION(BlueprintCallable) FWorkerSliders SetAttachmentWorkerCount(FWorkerSliders sliders, BuildingAttachments attachmentType);
	UFUNCTION(BlueprintCallable) FWorkersInHex GetWorkersInAttachment(BuildingAttachments attachment);

	UFUNCTION(BlueprintCallable) bool AttachmentIsActive(FText attachmentName);
	UFUNCTION(BlueprintCallable) bool AttachmentIsBuilt(BuildingAttachments attachment);

	UFUNCTION(BlueprintCallable) FBuildingDisplay GetAttachmentDisplay(FText attachmentName);
	UFUNCTION(BlueprintCallable) TArray<FBuildingDisplay> GetAttachmentDisplays();
	UFUNCTION(BlueprintCallable) FAttachmentBuildProgress GetAttachmentBuildProgress(BuildingAttachments attachment);
	UFUNCTION(BlueprintCallable) FAttachmentTTBuildInfo GetAttachmentBuildInfo(BuildingAttachments attachment);

	UFUNCTION(BlueprintCallable) FMaterialStorageStats GetMaterialStorageStats();
	UFUNCTION(BlueprintCallable) FRobotFactoryStats GetRobotFactoryStats();
	UFUNCTION(BlueprintCallable) FRobotStorageStats GetRobotStorageStats();
	UFUNCTION(BlueprintCallable) FDefenseStationStats GetDefenseStationStats();
private:
	AOutpost* GetOutpost();
#pragma endregion

#pragma region Hex Commands
public:
	UFUNCTION(BlueprintCallable) FHexDisplay GetHexDisplayInfo();
	UFUNCTION(BlueprintCallable) FCurrentResourceYields GetHexResourceYields();

	UFUNCTION(BlueprintCallable) FWorkersInHex GetWorkersInHex();
	UFUNCTION(BlueprintCallable) FWorkerSliders SetWorkerCount(FWorkerSliders sliders);

	UFUNCTION(BlueprintCallable) bool HexHasBuilding();
	UFUNCTION(BlueprintCallable) bool UnitTypeOnHex(UnitTypes type);

	UFUNCTION(BlueprintCallable) bool CanBuildOnHex();
	UFUNCTION(BlueprintCallable) bool CanPutWorkersOnHex();
	UFUNCTION(BlueprintCallable) bool HexIsDiscovered();
	UFUNCTION(BlueprintCallable) bool HexIsHumanControlled();
#pragma endregion

#pragma region Player Commands
public:
	UFUNCTION(BlueprintCallable) TArray<int> GetPlayerResources();
	UFUNCTION(BlueprintCallable) void SetPlayerResources(int foodCost, int prodCost, int energyCost, int wealthCost, int popCost, bool overrideCosts = true);
	UFUNCTION(BlueprintCallable) int GetResourceCap();
	UFUNCTION(BlueprintCallable) FResourcesPerTick GetResourcesPerTick();
	UFUNCTION(BlueprintCallable) FResourceGainLoss GetResourceRates();

	UFUNCTION(BlueprintCallable) int GetPlayerPopulation();
	UFUNCTION(BlueprintCallable) int GetStarveLevel();
	UFUNCTION(BlueprintCallable) int GetPowerOutageLevel();
	UFUNCTION(BlueprintCallable) FPlayerWorkers GetPlayerWorkers();

	UFUNCTION(BlueprintCallable) void ResourceCheats(int resourceToChange, int val);
#pragma endregion
	
#pragma region Troop Commands
public:
	UFUNCTION(BlueprintCallable) void SelectTroop(UnitTypes troopName);
	UFUNCTION(BlueprintCallable) void SplitArmyInHalf();
	UFUNCTION(BlueprintCallable) void ExtractUnitFromArmy(UnitTypes unitType);

	UFUNCTION(BlueprintCallable) bool CheckIfArmy();

	UFUNCTION(BlueprintCallable) TArray<FTroopDisplay> GetTroopDisplays();
	UFUNCTION(BlueprintCallable) FArmyDisplay DisplaySelectedUnit();
	UFUNCTION(BlueprintCallable) FArmyMenuInfo DisplayArmyMenu();

	FTroopUIData GetSimpleTroopUI(FUnitData* data);
	UFUNCTION(BlueprintCallable) FTroopTTInfo GetTroopTTDisplay(FText troopName);

	UFUNCTION(BlueprintCallable) void ChangeArmyName(FString newName);
	UFUNCTION(BlueprintCallable) FString GetTroopNameRaw();

	float GetTroopHPAlpha(FUnitData* unit = nullptr);
	float GetTroopMoraleAlpha(FUnitData* unit = nullptr);
	UTexture2D* GetTroopIcon(FUnitData* unit = nullptr);
private:
	FArmyDisplay DisplayUnit(FUnitData* unit) const;
#pragma endregion

#pragma region Battle Commands
public:
	UFUNCTION(BlueprintCallable) FTroopArmyDisplay GetBattleUnit(int group);
	UFUNCTION(BlueprintCallable) TArray<FTroopArmyDisplay> GetBattleUnits(int group);

	UFUNCTION(BlueprintCallable) void RetreatFromBattle();
	
	UFUNCTION(BlueprintCallable) bool BattleIsSiege();
	UFUNCTION(BlueprintCallable) FSiegeBuildingInfo GetSiegeBuildingInfo();

	UFUNCTION(BlueprintCallable) bool BattleGroupIsHuman(int group);
#pragma endregion

#pragma region Utility AI Hacks
public:
	template<typename T>
	inline void CommandTroop(ATroop* troop, T* target)
	{
		if constexpr (std::is_same<ABaseHex, T>::value)
		{
			ABaseHex* hex = Cast<ABaseHex>(target);
			if (hex->building && 
				hex->building->GetBuildingType() == SpawnableBuildings::RobotBarracks &&
				hex->GetHexOwner() == playerFaction &&
				IsInBuildMode())
			{
				troop->AI_SetMovementAction(troop_Store, hex->building->hexNav);
			}
			else
				troop->AI_SetMovementAction(troop_MoveToPos, hex);
		}
		else if constexpr (std::is_same<ATroop, T>::value)
		{
			ATroop* targetTroop = Cast<ATroop>(target);
			Factions targetFaction = targetTroop->GetUnitData()->GetFaction();

			if (targetFaction == playerFaction && IsInBuildMode())
				troop->AI_SetMovementAction(troop_Merge, targetTroop->hexNav);
			else
			{
				Faction* factionObject = UnitActions::GetFaction(playerFaction);
				FactionRelationship relation = factionObject->GetFactionRelationship(targetFaction);

				switch (relation)
				{
				case FactionRelationship::Enemy:
					troop->AI_SetMovementAction(troop_ChaseTarget, targetTroop->hexNav);
					break;
				default:
					troop->AI_SetMovementAction(troop_MoveToPos, targetTroop->hexNav->GetCurrentHex());
					break;
				}
			}
		}
	}
private:
	UPROPERTY(EditAnywhere, Category = "Utility AI") UAI_Action* troop_MoveToPos;
	UPROPERTY(EditAnywhere, Category = "Utility AI") UAI_Action* troop_Merge;
	UPROPERTY(EditAnywhere, Category = "Utility AI") UAI_Action* troop_ChaseTarget;
	UPROPERTY(EditAnywhere, Category = "Utility AI") UAI_Action* troop_HalveArmy;
	UPROPERTY(EditAnywhere, Category = "Utility AI") UAI_Action* troop_Store;
#pragma endregion
};
