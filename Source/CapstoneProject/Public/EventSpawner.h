// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventSpawner.generated.h"


UCLASS()
class CAPSTONEPROJECT_API AEventSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEventSpawner();

	struct EventStruct
	{
		UPROPERTY(EditAnywhere) FImage eventImage;
		UPROPERTY(EditAnywhere) FString eventTitle;
		UPROPERTY(EditAnywhere) FString eventDescript;
		UPROPERTY(EditAnywhere) FString eventButton01;
		UPROPERTY(EditAnywhere) FString eventButton02;
	};

	UFUNCTION(BlueprintCallable) void SpawnEvent(float deltaTime);

	//APlayerController* controller;
	//UUserWidget* eventWidget;
	//TSubclassOf<class UUserWidget> eventPrefab;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
