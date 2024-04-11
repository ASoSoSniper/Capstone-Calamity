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

	UPROPERTY(EditAnywhere) float buildTime = 5.f;
	float currBuildTime;
	
	UPROPERTY(VisibleAnywhere) AOutpost* outpost;
	UPROPERTY(VisibleAnywhere) BuildingAttachments type;
	
	BuildStates buildState = Inactive;

	TMap<WorkerType, int> workersInAttachment;
	int maxWorkers = 15;
	int GetNumberOfWorkers();

	void SetBuildState();
	void Constructing(float& DeltaTime);
	virtual bool SetUpAttachment(BuildingAttachments attachment);


	virtual void UpdateResources();

	virtual void ActivateAttachment();

	virtual void EnableAttachment();

	virtual void DisableAttachment();

	virtual bool AttachmentIsActive();
};
