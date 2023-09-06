// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovement.h"


// Sets default values
APlayerMovement::APlayerMovement()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(RootComponent);
	camera->SetWorldRotation(FRotator(-45.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void APlayerMovement::BeginPlay()
{
	Super::BeginPlay();
	
	AController* tempController = GetController();
	controller = Cast<ABasePlayerController>(tempController);
}

// Called every frame
void APlayerMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerMovement::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("DebugBuild", IE_Pressed, this, &APlayerMovement::BuildInput);
	PlayerInputComponent->BindAction("Deselect", IE_Pressed, this, &APlayerMovement::DeselectInput);
}

void APlayerMovement::BuildInput()
{
	if (controller) controller->Build();
}

void APlayerMovement::DeselectInput()
{
	if (controller) controller->Deselect();
}

