// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "SoundBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovement.generated.h"

class ABasePlayerController;

UCLASS()
class CAPSTONEPROJECT_API APlayerMovement : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerMovement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	enum ControlStates
	{
		Free,
		Cinematic
	};
	ControlStates controlState = Free;
	FCinematicObject targetCinemaPos;
	FVector startPos;
	float currCinemaTime;
	float currLingerTime;
	GameStates gameState = GameStates::None;

	void MoveToCinematicPos(float& DeltaTime);

	ABasePlayerController* controller;
	UPROPERTY(EditAnywhere) UCameraComponent* camera;
	UPROPERTY(EditAnywhere) UAudioComponent* audioComponent;
	UPROPERTY(EditAnywhere) UCharacterMovementComponent* movement;

	void Action1Input();
	void Action2Input();
	void Action3Input();
	void Action4Input();
	void Action5Input();
	void Action6Input();
	void Action7Input();
	void Action8Input();
	void Action9Input();
	void Action10Input();
	void Action11Input();
	void Action12Input();

	void DeselectInput();
	void SelectInput();
	void StartSelect();
	void StartCommand();
	
	void GetMouseX(float axis);
	void GetMouseY(float axis);

	void DragCamera();
	void PanRight(float axis);
	void PanUp(float axis);
	void ZoomIn(float axis);
	UPROPERTY(EditAnywhere) float cameraVel = 750.0f;
	float camMinX = -1000;
	float camMaxX = 1000;
	float camMinY = -1000;
	float camMaxY = 1000;
	float camMinZ = 100;
	float camMaxZ = 600;

	void AdjustTimeScale(float axis);

	UPROPERTY(EditAnywhere) float timeScaleIncrement = 0.5f;

	void HexCast();
	UPROPERTY(EditAnywhere) TSubclassOf<ASoundBox> soundBoxPrefab;
	UPROPERTY(EditAnywhere) ASoundBox* soundBox;

private:
	UPROPERTY(VisibleAnywhere) bool clickHeld = false;
	float mouseX = 0.f;
	float mouseY = 0.f;

	FVector2D previousMouseLocation;
	FVector2D currentMouseLocation;
};
