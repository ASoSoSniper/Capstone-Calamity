// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UnitActions.h"
#include "MeshVisibility.generated.h"

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

	enum VisibilityStatus
	{
		Undiscovered,
		Discovered,
		Visible
	};
	struct FVisibility
	{
		VisibilityStatus status = Undiscovered;
		bool inSight = false;
		bool discoveredByFaction = false;
	};
	struct FVisibilityMaterials
	{
		UMaterialInterface* hiddenTexture;
		UMaterialInterface* visibleTexture;
		UMaterialInterface* selectedTexture;
	};

	UPROPERTY(VisibleAnywhere) Factions faction;
	ObjectTypes objectType;
	FVisibilityMaterials meshMaterials;
	FVisibilityMaterials otherMeshMaterials;
	UStaticMeshComponent* mesh;
	UStaticMeshComponent* otherMesh;
	UPROPERTY(VisibleAnywhere) bool selected;
	UPROPERTY(EditAnywhere) bool debug;
	bool discoveredByPlayer;
	UPROPERTY(EditAnywhere) float visibilityRadius = 50.f;
	TMap<Factions, FVisibility> factionVisibility;

	void Scan(float radius);
	void InSight(Factions thisFaction);
	void SetVisibility();
};
