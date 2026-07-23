// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Objective.h"
#include "EventSystemManager.generated.h"

USTRUCT(BlueprintType)
struct FEventOption
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString title = "Quest for B (Rewards)";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true")) FString description = "By choosing B, you will have to complete these tasks: 1; 2; 3;";
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<UObjective*> objectives;
};

USTRUCT(BlueprintType)
struct FWorldEvent : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString title = "Choose a letter";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = "true")) FString description = "Do you choose A or B?";
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FEventOption> options;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventTriggered, const FWorldEvent&, event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEventClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveTriggered, UObjective*, objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectiveClosed);

struct FDateTickUpdate;

UCLASS()
class CAPSTONEPROJECT_API AEventSystemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEventSystemManager();
	virtual void Tick(float DeltaTime) override;

	static AEventSystemManager* GetEventSystemManager();
	static void TriggerEvent(FName eventKey);
	static FWorldEvent* GetEvent(FName eventKey);
	static void ScheduleEvent(int daysAhead, FName eventKey);

	UFUNCTION(BlueprintCallable) void CloseActiveEvent();
	UFUNCTION(BlueprintCallable) void CloseActiveObjective();
	UFUNCTION(BlueprintCallable) void SelectOption(const FEventOption& option);

	UPROPERTY(BlueprintAssignable) FOnEventTriggered onEventTriggered;
	UPROPERTY(BlueprintAssignable) FOnEventClosed onEventClosed;
	UPROPERTY(BlueprintAssignable) FOnObjectiveTriggered onObjectiveTriggered;
	UPROPERTY(BlueprintAssignable) FOnObjectiveClosed onObjectiveClosed;

protected:
	virtual void BeginPlay() override;

private:
	void CompleteObjective(UObjective* objective);
	void HandleDateTick(const FDateTickUpdate& date);

	static AEventSystemManager* eventManager;

	UFaction* playerFaction = nullptr;
	FWorldEvent* activeEvent = nullptr;
	TMap<long long int, FName> scheduledEvents;
	TQueue<FName> queuedEvents;

	UObjective* activeObjective;
	TSet<UObjective*> dockedObjectives;
	TQueue<UObjective*> queuedObjectives;
	

	UPROPERTY(EditAnywhere, Category = "Events") UDataTable* eventTable;
	static const FString eventSearchContext;
};
