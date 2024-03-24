// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "BaseHex.h"
#include "SoundBox.generated.h"

UCLASS()
class CAPSTONEPROJECT_API ASoundBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Components") USphereComponent* collider;
	UPROPERTY(VisibleAnywhere, Category = "Environment") TArray<ABaseHex*> foundHexes;
	UPROPERTY(EditAnywhere, Category = "Sounds") TMap<TerrainType, USoundBase*> terrainSounds;
	UPROPERTY(EditAnywhere, Category = "Sounds") float minVolume = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Sounds") float maxVolume = 1.f;
	UPROPERTY(EditAnywhere, Category = "Sounds") float fadeInSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Sounds") float fadeOutSpeed = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Sounds") float cameraZoomToTrigger = 200.f;
	bool soundsActive = false;

	void AssignHexes();
	void AdjustHexVolumes();
	void ToggleSoundsActive(bool active);

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
