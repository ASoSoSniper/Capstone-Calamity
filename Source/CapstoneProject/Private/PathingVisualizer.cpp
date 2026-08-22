// Fill out your copyright notice in the Description page of Project Settings.


#include "PathingVisualizer.h"
#include "CapstoneProjectGameModeBase.h"
#include "Faction.h"
#include "FactionController.h"
#include "PathingConditionDisplay.h"
#include "Components/ArrowComponent.h"
#include "UAI_PriorityManager_Troops.h"

// Sets default values
APathingVisualizer::APathingVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	arrowColors.Add(FColor::Red);
	arrowColors.Add(FColor::Blue);
	arrowColors.Add(FColor::Yellow);
	arrowColors.Add(FColor::Green);
	arrowColors.Add(FColor::Orange);
	arrowColors.Add(FColor::Cyan);
	arrowColors.Add(FColor::Emerald);
	arrowColors.Add(FColor::Magenta);
}

// Called when the game starts or when spawned
void APathingVisualizer::BeginPlay()
{
	Super::BeginPlay();

	BindFactions();
}

void APathingVisualizer::BindFactions()
{
	const TMap<EFactions, UFaction*>& factions = ACapstoneProjectGameModeBase::activeFactions;

	for (EFactions f : trackedFactions)
	{
		if (!factions.Contains(f)) continue;

		factions[f]->onTroopAdded.AddDynamic(this, &APathingVisualizer::HandleOnTroopAdded);
		factions[f]->onTroopRemoved.AddDynamic(this, &APathingVisualizer::HandleOnTroopRemoved);

		UUAI_PriorityManager_Troops* priorityManager = factions[f]->GetFactionController()->GetPriorityManager_Troops();

		priorityManager->onTroopSelected.AddDynamic(this, &APathingVisualizer::HandleOnTroopPrioritized);
		priorityManager->onHexSelected.AddDynamic(this, &APathingVisualizer::HandleOnHexPrioritized);
	}
}

void APathingVisualizer::HandleOnTroopAdded(ATroop* troop)
{
	if (trackedTroops.Contains(troop)) return;

	trackedTroops.Add(troop, TArray<AActor*>());
	troop->onDestinationSet.AddDynamic(this, &APathingVisualizer::HandleOnDestinationSet);
}

void APathingVisualizer::HandleOnTroopRemoved(ATroop* troop)
{
	if (!trackedTroops.Contains(troop)) return;

	trackedTroops.Remove(troop);
	troop->onDestinationSet.RemoveDynamic(this, &APathingVisualizer::HandleOnDestinationSet);
}

void APathingVisualizer::HandleOnDestinationSet(const AMovementAI* troop, const ABaseHex* prevHex)
{
	const ATroop* asTroop = Cast<const ATroop>(troop);
	if (!asTroop || !trackedTroops.Contains(asTroop)) return;

	if (!trackedTroops[asTroop].IsEmpty())
	{
		for (int i = 0; i < trackedTroops[asTroop].Num(); i++)
		{
			trackedTroops[asTroop][i]->Destroy();
			trackedTroops[asTroop][i] = nullptr;
		}

		trackedTroops[asTroop].Empty();
	}

	const TArray<const ABaseHex*>& path = troop->GetHexPath();

	FColor color = arrowColors[arrowColorIndex];
	arrowColorIndex++;
	if (arrowColorIndex >= arrowColors.Num()) arrowColorIndex = 0;

	for (const ABaseHex* hex : path)
	{
		AActor* marker = GetWorld()->SpawnActor(pathStepMarker);
		marker->SetActorLocation(hex->GetActorLocation());
		marker->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		UArrowComponent* arrow = marker->FindComponentByClass<UArrowComponent>();
		if (arrow)
			arrow->SetArrowFColor(color);

		trackedTroops[asTroop].Add(marker);
	}

	const ABaseHex* target = troop->hexNav->GetTargetHex();
	FVector2D coords = target->GetHexCoordinates();
	FString hexName = FString::Printf(TEXT("--> Hex: (%d, %d) <--"), static_cast<int>(coords.X), static_cast<int>(coords.Y));
	FString troopName = "--> Troop: " + troop->GetTroopName().ToString() + " <--";

	TArray<FString> displayInput;
	displayInput.Add(hexName);
	displayInput.Append(priorityHexConditions);
	displayInput.Add(troopName);
	displayInput.Append(priorityTroopConditions);

	APathingConditionDisplay* conditionDisplay = GetWorld()->SpawnActor<APathingConditionDisplay>(pathConditionDisplay);
	conditionDisplay->SetActorLocation(target->GetActorLocation());
	conditionDisplay->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	conditionDisplay->Initialize(displayInput);

	trackedTroops[asTroop].Add(conditionDisplay);
}

void APathingVisualizer::HandleOnTroopPrioritized(ATroop* troop, const TArray<FString>& conditions)
{
	priorityTroopConditions = conditions;
}

void APathingVisualizer::HandleOnHexPrioritized(ABaseHex* hex, const TArray<FString>& conditions)
{
	priorityHexConditions = conditions;
}

