// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_BuildingProgressBar.h"

void UUI_BuildingProgressBar::BeginPlay()
{
	parentBuilding = Cast<ABuilding>(GetOwner());
}

void UUI_BuildingProgressBar::SetProgressBar()
{
	if (!IsBuilding() && progressBar->Visibility == ESlateVisibility::Hidden) return;

	if (progressBar->Visibility != ESlateVisibility::HitTestInvisible) 
		progressBar->SetVisibility(ESlateVisibility::HitTestInvisible);

	float percent = (parentBuilding->buildTime - parentBuilding->currBuildTime) / parentBuilding->buildTime;

	progressBar->SetPercent(percent);

	if (percent >= 1.f)
	{
		progressBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UUI_BuildingProgressBar::IsBuilding()
{
	if (!parentBuilding) return false;
	return parentBuilding->buildState == ABuilding::Building;
}

void UUI_BuildingProgressBar::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SetProgressBar();
}
