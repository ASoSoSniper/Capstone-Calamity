// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "BaseHex.h"
#include "HexNav.h"
#include "UnitStats.h"
#include "MeshVisibility.h"
#include "Components/SphereComponent.h"
#include "ExploreAnims.h"
#include "MovementAI.generated.h"

UCLASS()
class CAPSTONEPROJECT_API AMovementAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovementAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void CreatePath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere) UInteractable* interact;
	UPROPERTY(VisibleAnywhere) UHexNav* hexNav;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* mesh;
	UPROPERTY(EditAnywhere) UUnitStats* unitStats;
	UPROPERTY(EditAnywhere) UMeshVisibility* visibility;
	bool selectedByPlayer;

	void SetDestination(AActor* targetHex);
	void SphereCheck(float rangeMulti = 1.f);
	virtual void CancelPath();

protected:
	UPROPERTY(VisibleAnywhere) TArray<AActor*> hexPath;
	int hexPathIndex;
	UPROPERTY(VisibleAnywhere) float currTimeTillHexMove = 0.f;

	void CountdownToMove(float& DeltaTime);
	virtual void MoveToTarget(float& DeltaTime);
	
	virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere) float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere) float traceLength = 50.f;
	UPROPERTY(EditAnywhere) float hexSearchDistance = 100.f;
	UPROPERTY(EditAnywhere) float hexSnapDistance = 2.f;
	UPROPERTY(EditAnywhere) UExploreAnims* anims;
	UPROPERTY(EditAnywhere) int maxHexes = 200;
	UPROPERTY(EditAnywhere) float moveSpeed = 1.f;
	float currentMoveAlpha = 0.f;

	float AngleBetweenVectors(FVector a, FVector b);
	FVector GetVectorToTarget(FVector origin);
	bool HexIsTraversable(AActor* hex);
	bool HexIsTraversable(ABaseHex* hex);
	void SnapToHex(ABaseHex* hex);
	ABaseHex* HexSearch(AActor* hex);

	TArray<AActor*> GeneratePath(ABaseHex* destination);

	enum MoveStates
	{
		Idle,
		Move,
		Attack
	};
	MoveStates moveState = Idle;
};
