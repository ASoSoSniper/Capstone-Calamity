// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitActions.h"
#include "GlobalSpawner.h"
#include "MeshVisibility.generated.h"

UENUM()
enum class VisibilityStatus
{
	Undiscovered,
	Discovered,
	Visible
};

USTRUCT()
struct FVisibility
{
	GENERATED_BODY()

	UPROPERTY() VisibilityStatus status = VisibilityStatus::Undiscovered;
	UPROPERTY() bool inSight = false;
	UPROPERTY() bool discoveredByFaction = false;
};

UENUM(BlueprintType)
enum class EVisibilityColor
{
	Default,
	Hidden,
	Selected
};

USTRUCT(BlueprintType)
struct FVisibilityFade
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) FColor color = FColor::White;
	UPROPERTY(EditAnywhere) float duration = 0.5f;
	UPROPERTY(EditAnywhere) UCurveFloat* fadeCurve = nullptr;
};

class UHexNav;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UMeshVisibility : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeshVisibility();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere) Factions faction;

	UPROPERTY(VisibleAnywhere, Category = "Functionality") bool enableScan = true;

	void SetSelected(bool active, bool instigator = true);
	bool VisibleToFaction(Factions factionToCheck) const;
	bool DiscoveredByFaction(Factions factionToCheck) const;

	void SetupComponent(Factions setFaction, UMeshComponent* meshComponent);
	void SetupComponent(FUnitData* data, UMeshComponent* meshComponent);
	void SetupFactionComponent(UMeshComponent* meshComponent);
	void ResetComponent();
	
private:
	UPROPERTY(EditAnywhere, Category = "Colors") TMap<EVisibilityColor, FVisibilityFade> visibilityColors;
	UPROPERTY(VisibleAnywhere, Category = "Colors") EVisibilityColor colorTarget = EVisibilityColor::Hidden;
	UPROPERTY(VisibleAnywhere, Category = "Colors") EVisibilityColor prevColorTarget = EVisibilityColor::Hidden;
	UPROPERTY(VisibleAnywhere, Category = "Colors") float colorAlpha = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Functionality") ObjectTypes objectType;
	UPROPERTY(VisibleAnywhere, Category = "Functionality") bool selected;

	UPROPERTY(EditAnywhere, Category = "Debug") bool debug;
	UPROPERTY(EditAnywhere, Category = "Debug") bool infiniteRange;
	UPROPERTY(EditAnywhere, Category = "Debug") bool showDebugSphere = false;

	UPROPERTY() TArray<UMaterialInstanceDynamic*> meshMaterials;
	UPROPERTY() UMaterialInstanceDynamic* factionMat;
	TMap<Factions, FVisibility> factionVisibility;
	FUnitData* unitData;
	UPROPERTY() UHexNav* hexNav;
	UPROPERTY() ABaseHex* hexParent;
	UPROPERTY() bool discoveredByPlayer;

	void Scan();
	void InSight(Factions thisFaction);
	bool SetVisibility();
	void RevealModelsAndMeshes();
	void HideModelsAndMeshes();

	void SetColorTarget(EVisibilityColor setTarget);
	void FadeToColor(const float& DeltaTime);
	void ToggleOpacity(bool active);
};
