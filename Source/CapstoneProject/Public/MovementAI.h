// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "BaseHex.h"
#include "HexNav.h"
#include "UnitStats.h"
#include "Components/SphereComponent.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		UInteractable* interact;
	UPROPERTY(VisibleAnywhere)
		UHexNav* hexNav;
	UPROPERTY(VisibleAnywhere)
		TArray<AActor*> hexPath;
	int hexPathIndex;
	
	UPROPERTY(EditAnywhere)
		float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere)
		float traceLength = 50.f;
	UPROPERTY(EditAnywhere)
		float hexSnapDistance = 5.f;
	UPROPERTY(EditAnywhere)
		int maxHexes = 16;
	UPROPERTY(EditAnywhere)
		float moveSpeed = 2.f;
	virtual void CreatePath();
	void SnapToHex(ABaseHex* hex);
	ABaseHex* HexSearch(AActor* hex);
	
	void SphereCheck(float rangeMulti = 1.f);

	float AngleBetweenVectors(FVector a, FVector b);
	FVector GetVectorToTarget(FVector origin);

	enum MoveStates
	{
		Idle,
		Move,
		Attack
	};
	MoveStates moveState = Idle;
	virtual void MoveToTarget(float& DeltaTime);
	
	UPROPERTY(EditAnywhere)
		USphereComponent* sphere;
	UPROPERTY(EditAnywhere)
		UUnitStats* unitStats;
};
