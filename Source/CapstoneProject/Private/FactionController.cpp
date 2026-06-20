// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionController.h"
#include "CapstoneProjectGameModeBase.h"
#include "Faction.h"
#include "UAI_PriorityManager_Hex.h"


// Sets default values
AFactionController::AFactionController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	priorityManager_Hex = CreateDefaultSubobject<UUAI_PriorityManager_Hex>(TEXT("Hex Priority Manager"));
	decider_Resources = CreateDefaultSubobject<UUAI_Decider>(TEXT("Resource Decider"));
	decider_Troops = CreateDefaultSubobject<UUAI_Decider>(TEXT("Troop Decider"));
}

// Called when the game starts or when spawned
void AFactionController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFactionController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFactionController::SetFaction(UFaction* setFaction)
{
	if (faction != nullptr) return;

	faction = setFaction;

	ACapstoneProjectGameModeBase::onDateTick.AddDynamic(this, &AFactionController::DateUpdate);

	priorityManager_Hex->Initialize(faction);
	decider_Resources->Initialize(faction);
	decider_Troops->Initialize(faction);
}

UFaction* AFactionController::GetFactionObject() const
{
	return faction;
}

bool AFactionController::IsAIControlled()
{
	return faction->GetFaction() != EFactions::Human;
}

void AFactionController::TriggerUpdateDisplay()
{
	decider_Resources->onDecisionMade.Broadcast();
}

void AFactionController::DateUpdate(const FDateTickUpdate& update)
{
	if (update.minuteTick)
	{
		
	}
	if (update.hourTick)
	{
		faction->UpdateResourceCosts();
	}
	if (update.dayTick)
	{
		faction->FeedPop();
		faction->ConsumeEnergy();
	}
	if (update.monthTick)
	{

	}
}

bool AFactionController::UpdateDisplay()
{
	if (updateDecisionDisplay)
	{ 
		updateDecisionDisplay = false;
		return true;
	}

	return false;
}

UUAI_Decider* AFactionController::GetResourceDecider()
{
	return decider_Resources;
}

UUAI_Decider* AFactionController::GetTroopDecider()
{
	return decider_Troops;
}

#pragma region Priority Targeting
void AFactionController::BindHexDelegates(ABaseHex* hex, bool enable)
{
	priorityManager_Hex->BindHexDelegates(hex, enable);
}
void AFactionController::BindBuildingDelegates(ABuilding* building, bool enable)
{
	priorityManager_Hex->BindBuildingDelegates(building, enable);
}
ABaseHex* AFactionController::GetPriorityHex_Workers(EStratResources resource) const
{
	return priorityManager_Hex->GetPriorityHex_Workers(resource);
}
ABaseHex* AFactionController::GetPriorityHex_Building(SpawnableBuildings building) const
{
	return priorityManager_Hex->GetPriorityHex_Building(building);
}
#pragma endregion