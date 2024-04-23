// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "MusicObject.generated.h"

UCLASS()
class CAPSTONEPROJECT_API AMusicObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere) UAudioComponent* audio;

	UPROPERTY(EditAnywhere) TArray<USoundBase*> music;
	int musicIndex = 0;
	UPROPERTY(EditAnywhere) float timeBetweenSongs;
	float currentTime;
};
