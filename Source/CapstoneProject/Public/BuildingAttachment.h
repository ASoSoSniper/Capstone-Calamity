// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Outpost.h"
#include "BuildingAttachment.generated.h"

UENUM()
enum BuildStates
{
	Inactive,
	Disabled,
	Building,
	Complete
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UBuildingAttachment : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuildingAttachment();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TMap<WorkerType, int> workersInAttachment;
	
	BuildStates GetBuildState();

	virtual void ActivateAttachment(bool instantBuild = false);

	virtual void EnableAttachment();

	virtual bool DisableAttachment();

	virtual bool AttachmentIsActive();
	virtual bool AttachmentIsBuilt();

	int GetNumberOfWorkers();
	int GetMaxWorkers();
	bool WorkersAtCap();

	float GetBuildTime();
	float GetCurrentBuildTime();

protected:
	UPROPERTY(VisibleAnywhere) AOutpost* outpost;
	BuildingAttachments attachmentType;
	BuildStates buildState = BuildStates::Inactive;
	void SetBuildState();
	void Constructing(float& DeltaTime);
	virtual bool SetUpAttachment(BuildingAttachments attachment);

	virtual void UpdateResources();

	float buildTime = 5.f;
	float currBuildTime;

	int maxWorkers = 15;
};
