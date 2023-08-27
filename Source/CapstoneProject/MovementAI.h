// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	AActor* currentHex;
	TArray<AActor*> hexTrail;
	AActor* targetHex;
	UPROPERTY(EditAnywhere)
		float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere)
		float traceLength = 10.f;
	void SnapToHex(AActor* hex);
	void HexSearch(AActor* hex);
	float AngleBetweenVectors(const FVector& a, const FVector& b);
	FVector GetVectorToTarget(FVector& origin);

	UPROPERTY(EditAnywhere)
		AActor* TestActor;

};
