// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ShapeComponent.h"
#include "Interactable.generated.h"

class ABasePlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UInteractable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	ABasePlayerController* controller;
	UFUNCTION() //This makes the function actually trigger in AddDynamic, DO NOT FORGET
	void MouseHover(UPrimitiveComponent* item);

	UFUNCTION()
	void Selected(UPrimitiveComponent* item, FKey ButtonPressed);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* collider;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* otherCollider;
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* skeletalMesh;
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* otherSkeletalMesh;
	UPROPERTY(EditAnywhere)
	bool canInteract = true;

	void CreateCollision(UStaticMeshComponent* mesh);
	void CreateCollision(USkeletalMeshComponent* mesh);
	void CreateExtraCollision(UStaticMeshComponent* mesh);
	void CreateExtraCollision(USkeletalMeshComponent* mesh);
	bool SetInteract(bool active);
};
