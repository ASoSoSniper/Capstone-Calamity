// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingAttachment.h"
#include "CapstoneProjectGameModeBase.h"


// Sets default values for this component's properties
UBuildingAttachment::UBuildingAttachment()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	workersInAttachment.Add(WorkerType::Human, 0);
	workersInAttachment.Add(WorkerType::Robot, 0);
	workersInAttachment.Add(WorkerType::Alien, 0);
}


// Called when the game starts
void UBuildingAttachment::BeginPlay()
{
	Super::BeginPlay();

	currBuildTime = buildTime;
	outpost = Cast<AOutpost>(GetOwner());
}


// Called every frame
void UBuildingAttachment::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	switch (buildState)
	{
	case Building:
		Constructing(DeltaTime);
		break;
	case Complete:
		break;

	}
}

int UBuildingAttachment::GetNumberOfWorkers()
{
	int totalWorkers = 0;

	for (auto worker : workersInAttachment)
	{
		totalWorkers += worker.Value;
	}

	return totalWorkers;
}

void UBuildingAttachment::SetBuildState()
{
	switch (buildState)
	{
	case Building:
		buildState = Complete;
		UpdateResources();
		break;
	case Complete:
		break;
	}
}

void UBuildingAttachment::Constructing(float& DeltaTime)
{
	if (currBuildTime > 0)
	{
		currBuildTime -= DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		return;
	}

	SetBuildState();
}

bool UBuildingAttachment::SetUpAttachment(BuildingAttachments attachment)
{
	return false;
}

void UBuildingAttachment::UpdateResources()
{
	
}

void UBuildingAttachment::ActivateAttachment()
{
	SetUpAttachment(type);
	buildState = Building;
}

void UBuildingAttachment::EnableAttachment()
{
}

void UBuildingAttachment::DisableAttachment()
{
	buildState = Inactive;
}

bool UBuildingAttachment::AttachmentIsActive()
{
	return buildState == Complete;
}

