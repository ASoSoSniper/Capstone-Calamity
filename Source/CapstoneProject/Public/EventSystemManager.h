// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Objective.h"
#include "EventSystemManager.generated.h"

USTRUCT(BlueprintType)
struct FEventOption
{
	GENERATED_USTRUCT_BODY();

public:
	FString GetOptionTitle() const;
	FString GetOptionDescription() const;
	const TArray<UObjective*>& GetObjectives() const;

private:
	UPROPERTY(EditAnywhere) FString title = "Quest for B (Rewards)";
	UPROPERTY(EditAnywhere) FString description = "By choosing B, you will have to complete these tasks: 1; 2; 3;";
	TArray<UObjective*> objectives;
};

USTRUCT(BlueprintType)
struct FWorldEvent
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere) FString title = "Choose a letter";
	UPROPERTY(EditAnywhere) FString description = "Do you choose A or B?";
	UPROPERTY(EditAnywhere) UTexture2D* image;
	UPROPERTY(EditAnywhere) TArray<FEventOption> options;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEventTriggered, const FWorldEvent&, event);

UCLASS()
class CAPSTONEPROJECT_API AEventSystemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEventSystemManager();
	virtual void Tick(float DeltaTime) override;

	static void TriggerEvent(FWorldEvent event);

	UFUNCTION(BlueprintCallable) void CloseActiveEvent();
	UFUNCTION(BlueprintCallable) void SelectOption(FEventOption option);

	UPROPERTY(BlueprintAssignable) FOnEventTriggered onEventTriggered;

protected:
	virtual void BeginPlay() override;

private:
	void CompleteObjective(UObjective* objective);

	static AEventSystemManager* eventManager;

	UFaction* playerFaction = nullptr;
	FWorldEvent* activeEvent = nullptr;
	TSet<UObjective*> dockedObjectives;
};
