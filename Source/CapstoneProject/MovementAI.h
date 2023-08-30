// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
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

	UInteractable* interact;

	AActor* currentHex;
	TArray<AActor*> hexPath;
	AActor* targetHex;
	UPROPERTY(EditAnywhere)
		float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere)
		float traceLength = 10.f;
	UPROPERTY(EditAnywhere)
		int maxHexes;

	void CreatePath();
	void SnapToHex(AActor* hex);
	AActor* HexSearch(AActor* hex);
	float AngleBetweenVectors(FVector a, FVector b);
	FVector GetVectorToTarget(FVector origin);

	UPROPERTY(EditAnywhere)
		AActor* TestActor;

	UPROPERTY(EditAnywhere)
		USphereComponent* sphere;

};
