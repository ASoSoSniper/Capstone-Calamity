// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshVisibility.h"
#include "HexNav.h"
#include "CapstoneProjectGameModeBase.h"

// Sets default values for this component's properties
UMeshVisibility::UMeshVisibility()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	visibilityColors.Add(EVisibilityColor::Default, FVisibilityFade{ FColor::White, 0.5f });
	visibilityColors.Add(EVisibilityColor::Hidden, FVisibilityFade{ FColor(0.1f, 0.1f, 0.1f, 1.f), 0.5f });
	visibilityColors.Add(EVisibilityColor::Selected, FVisibilityFade{ FColor(1.f, 1.f, 0.f, 1.f), 0.1f });
}


// Called when the game starts
void UMeshVisibility::BeginPlay()
{
	Super::BeginPlay();

	objectType = UnitActions::DetermineObjectType(GetOwner()).type;
	if (objectType == ObjectTypes::Hex)
		hexParent = Cast<ABaseHex>(GetOwner());
	else
		hexNav = GetOwner()->GetComponentByClass<UHexNav>();
	
	for (auto& curFaction : ACapstoneProjectGameModeBase::activeFactions)
	{
		factionVisibility.Add(curFaction.Key, FVisibility{ VisibilityStatus::Undiscovered, false, false });
	}
}

void UMeshVisibility::SetupComponent(Factions setFaction, UMeshComponent* meshComponent)
{
	faction = setFaction;

	if (!meshComponent) return;
	
	UMaterialInterface* material = meshComponent->GetMaterial(0);
	UMaterialInstanceDynamic* dynamic = UMaterialInstanceDynamic::Create(material, meshComponent->GetOwner());

	meshMaterials.Add(dynamic);
	meshComponent->SetMaterial(0, dynamic);
}

void UMeshVisibility::SetupComponent(FUnitData* data, UMeshComponent* meshComponent)
{
	unitData = data;
	
	SetupComponent(data->GetFaction(), meshComponent);
}

void UMeshVisibility::SetupFactionComponent(UMeshComponent* meshComponent)
{
	if (!meshComponent || factionMat) return;

	meshComponent->EmptyOverrideMaterials();

	UMaterialInterface* material = meshComponent->GetMaterial(0);
	UMaterialInstanceDynamic* dynamic = UMaterialInstanceDynamic::Create(material, meshComponent);

	factionMat = dynamic;
	meshComponent->SetMaterial(0, dynamic);
}

void UMeshVisibility::ResetComponent()
{
	faction = Factions::None;
	meshMaterials.Empty();
	factionMat = nullptr;
}

// Called every frame
void UMeshVisibility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (factionVisibility.Num() < ACapstoneProjectGameModeBase::activeFactions.Num())
	{
		for (auto& curFaction : ACapstoneProjectGameModeBase::activeFactions)
		{
			if (!factionVisibility.Contains(curFaction.Key))
				factionVisibility.Add(curFaction.Key, FVisibility{ VisibilityStatus::Undiscovered, false, false });
		}
		return;
	}

	FadeToColor(DeltaTime);

	if (ACapstoneProjectGameModeBase::currentScanTime <= 0.f)
	{
		if (enableScan) Scan();

		if (SetVisibility())
			RevealModelsAndMeshes();
		else
			HideModelsAndMeshes();
	}
}

void UMeshVisibility::Scan()
{
	if (!hexNav && !hexParent) return;

	int baseVision = (unitData != nullptr) ? unitData->GetVision() : 1;
	float hexVisionMod = (hexNav && hexNav->GetCurrentHex()) ? hexNav->GetCurrentHex()->GetVision() : 0;

	int searchRadius = FMath::Max(baseVision + hexVisionMod, 0);

	ABaseHex* centerHex = hexNav ? hexNav->GetCurrentHex() : hexParent;
	TSet<ABaseHex*> foundHexes = centerHex->GetHexesInRadius(searchRadius);

	for (ABaseHex* hex : foundHexes)
	{
		hex->visibility->InSight(faction);

		if (hex->building)
			hex->building->visibility->InSight(faction);
		if (hex->battle)
			hex->battle->visibility->InSight(faction);
		if (!hex->troopsInHex.IsEmpty())
		{
			for (int i = 0; i < hex->troopsInHex.Num(); i++)
			{
				hex->troopsInHex[i]->visibility->InSight(faction);
			}
		}
	}
}

void UMeshVisibility::InSight(Factions thisFaction)
{
	//Add faction to object if it doesn't already exist
	if (!factionVisibility.Contains(thisFaction))
	{
		factionVisibility.Add(thisFaction, FVisibility{ VisibilityStatus::Undiscovered, false, false });
	}

	//Set object as "In Sight" for the faction and all its allies
	for (auto& ally : ACapstoneProjectGameModeBase::activeFactions)
	{
		UFaction* factionObject = UnitActions::GetFaction(thisFaction);
		if (!factionObject) continue;

		if (factionObject->GetFactionRelationship(ally.Key) == FactionRelationship::Ally)
		{
			factionVisibility[ally.Key].inSight = true;
		}
	}
}

bool UMeshVisibility::SetVisibility()
{
	bool visibleToPlayer = false;

	for (auto& curfaction : factionVisibility)
	{
		if (curfaction.Value.inSight)
		{
			factionVisibility[curfaction.Key].discoveredByFaction = true;
			factionVisibility[curfaction.Key].status = VisibilityStatus::Visible;
			factionVisibility[curfaction.Key].inSight = false;

			UnitActions::SetTargetListElement(curfaction.Key, GetOwner());

			if (UnitActions::GetFaction(Factions::Human)->GetFactionRelationship(curfaction.Key) == FactionRelationship::Ally)
			{
				visibleToPlayer = true;
				if (debug) GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Visible to player"));
			}
		}
		else
		{
			factionVisibility[curfaction.Key].status = factionVisibility[curfaction.Key].discoveredByFaction ? VisibilityStatus::Discovered : VisibilityStatus::Undiscovered;
		}
	}

	//If the player commands this object and it is not a hex, set visible to player by default
	if (UnitActions::GetFaction(Factions::Human)->GetFactionRelationship(faction) == FactionRelationship::Ally
		&& objectType != ObjectTypes::Hex) visibleToPlayer = true;

	return visibleToPlayer;
}

void UMeshVisibility::RevealModelsAndMeshes()
{
	EVisibilityColor target = selected ? EVisibilityColor::Selected : EVisibilityColor::Default;

	switch (objectType)
	{
	case ObjectTypes::Hex:
		SetColorTarget(target);
		if (hexParent->building)
			hexParent->building->visibility->SetColorTarget(target);

		if (!discoveredByPlayer && factionVisibility[Factions::Human].discoveredByFaction)
		{
			hexParent->SetHexModel();
			discoveredByPlayer = true;
			if (debug) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Discovered by player"));
		}
		hexParent->ToggleUI(true);
		if (VisibleToFaction(Factions::Human) && faction != Factions::None)
		{
			if (FFactionDisplay* display = AGlobalSpawner::spawnerObject->GetFactionDisplayPreset(faction))
				factionMat->SetVectorParameterValue(FName("Visibility"), display->tileColor);
		}
		break;

	case ObjectTypes::Building:
		ToggleOpacity(true);
		break;
	case ObjectTypes::MoveAI:
		SetColorTarget(target);
		ToggleOpacity(true);
		break;
	}
}
void UMeshVisibility::HideModelsAndMeshes()
{
	SetColorTarget(EVisibilityColor::Hidden);

	switch (objectType)
	{
	case ObjectTypes::Hex:
		hexParent->ToggleUI(false);
		break;

	case ObjectTypes::MoveAI:
		ToggleOpacity(false);
		break;

	case ObjectTypes::Building:
		if (!factionVisibility[Factions::Human].discoveredByFaction)
		{
			ToggleOpacity(false);
		}
		break;
	}
}

void UMeshVisibility::SetColorTarget(EVisibilityColor setTarget)
{
	if (setTarget == colorTarget) return;

	prevColorTarget = colorTarget;
	colorTarget = setTarget;
	colorAlpha = 0.f;
}

void UMeshVisibility::FadeToColor(const float& DeltaTime)
{
	if (!visibilityColors.Contains(prevColorTarget) ||
		!visibilityColors.Contains(colorTarget)) return;

	FVisibilityFade& start = visibilityColors[prevColorTarget];
	FVisibilityFade& end = visibilityColors[colorTarget];

	FLinearColor startColor = start.color;
	FLinearColor endColor = end.color;

	colorAlpha += DeltaTime / end.duration;
	colorAlpha = FMath::Clamp(colorAlpha, 0.f, 1.f);

	float alpha = end.fadeCurve ? end.fadeCurve->GetFloatValue(colorAlpha) : colorAlpha;
	FLinearColor setColor = FMath::Lerp(startColor, endColor, alpha);

	for (UMaterialInstanceDynamic* material : meshMaterials)
	{
		material->SetVectorParameterValue(FName("Visibility"), setColor);
	}
}

void UMeshVisibility::ToggleOpacity(bool active)
{
	for (UMaterialInstanceDynamic* material : meshMaterials)
	{
		material->SetScalarParameterValue(FName("Opacity"), active);
	}
}

void UMeshVisibility::SetSelected(bool active, bool instigator)
{
	selected = active;

	if (!instigator) return;

	if (objectType == ObjectTypes::Hex)
	{
		if (!hexParent->building) return;

		ABaseHex* centerHex = hexParent->building->hexNav->GetCurrentHex();
		int hexRadius = hexParent->building->GetHexLayersToOccupy();

		TSet<ABaseHex*> occupiedHexes = centerHex->GetHexesInRadius(hexRadius);
		for (ABaseHex* hex : occupiedHexes)
		{
			hex->visibility->SetSelected(selected, false);
		}
	}
}

bool UMeshVisibility::VisibleToFaction(Factions factionToCheck) const
{
	if (!factionVisibility.Contains(factionToCheck)) return false;

	return factionVisibility[factionToCheck].status == VisibilityStatus::Visible;
}

bool UMeshVisibility::DiscoveredByFaction(Factions factionToCheck) const
{
	if (!factionVisibility.Contains(factionToCheck)) return false;

	switch (factionVisibility[factionToCheck].status)
	{
	case VisibilityStatus::Undiscovered:
		return false;
	default:
		return true;
	}
}
