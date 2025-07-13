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
	
	FVisibilityMaterials meshMaterials;
	FVisibilityMaterials hexBaseMaterials;
	UPROPERTY() UStaticMeshComponent* mesh;
	UPROPERTY() UStaticMeshComponent* otherMesh;
	UPROPERTY() USkeletalMeshComponent* skeletalMesh;
	UPROPERTY() USkeletalMeshComponent* otherSkeletalMesh;
	UPROPERTY() UStaticMeshComponent* hexBaseMesh;
	UPROPERTY(VisibleAnywhere) Factions faction;
	UPROPERTY(VisibleAnywhere) bool enableScan = true;
	UPROPERTY(EditAnywhere) float visibilityRadius = 100.f;
	UPROPERTY(EditAnywhere) float detectionDistanceInRadius = 90.f;
	TMap<Factions, FVisibility> factionVisibility;

	void SetSelected(bool active);
	bool VisibleToFaction(Factions factionToCheck);
	bool DiscoveredByFaction(Factions factionToCheck);

private:
	
	UPROPERTY(VisibleAnywhere) ObjectTypes objectType;
	UPROPERTY() UUnitStats* unitStats;
	UPROPERTY() UHexNav* hexNav;
	
	UPROPERTY(VisibleAnywhere) bool selected;
	UPROPERTY(EditAnywhere) bool debug;
	UPROPERTY() bool discoveredByPlayer;
	UPROPERTY(EditAnywhere) bool infiniteRange;
	UPROPERTY(EditAnywhere) bool showDebugSphere = false;

	void FindFactionOfOwner();
	void Scan(float radius);
	void InSight(Factions thisFaction);
	void SetVisibility();
};
