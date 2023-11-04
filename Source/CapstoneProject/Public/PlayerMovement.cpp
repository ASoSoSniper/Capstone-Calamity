// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovement.h"
#include "CapstoneProjectGameModeBase.h"


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

	PlayerInputComponent->BindAction("Action1", IE_Pressed, this, &APlayerMovement::Action1Input);
	PlayerInputComponent->BindAction("Action2", IE_Pressed, this, &APlayerMovement::Action2Input);
	PlayerInputComponent->BindAction("Action3", IE_Pressed, this, &APlayerMovement::Action3Input);
	PlayerInputComponent->BindAction("Action4", IE_Pressed, this, &APlayerMovement::Action4Input);

	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerMovement::PanRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerMovement::PanUp);
	PlayerInputComponent->BindAxis("ZoomIn", this, &APlayerMovement::ZoomIn);

	PlayerInputComponent->BindAction("Deselect", IE_Pressed, this, &APlayerMovement::DeselectInput);

	PlayerInputComponent->BindAction("TimeScaleIncrease", IE_Pressed, this, &APlayerMovement::SpeedUpTime);
	PlayerInputComponent->BindAction("TimeScaleDecrease", IE_Pressed, this, &APlayerMovement::SlowDownTime);
}

void APlayerMovement::Action1Input()
{
	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action1();
	}
}

void APlayerMovement::Action2Input()
{
	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action2();
	}
}

void APlayerMovement::Action3Input()
{
	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action3();
	}
}

void APlayerMovement::Action4Input()
{
	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action4();
	}
}

void APlayerMovement::DeselectInput()
{
	if (controller) controller->Deselect();
}

void APlayerMovement::PanRight(float axis)
{
	if (GetActorLocation().Y >= camMinY && GetActorLocation().Y <= camMaxY)
	{
		FVector currLocation = GetActorLocation();
		currLocation += FVector::RightVector * axis * cameraVel * FApp::GetDeltaTime() * -1.0;
		if (currLocation.Y > camMaxY)
		{
			currLocation.Y = camMaxY;
		}
		else if (currLocation.Y < camMinY)
		{
			currLocation.Y = camMinY;
		}
		SetActorLocation(currLocation);
		GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::Printf(TEXT("X pos = %f"), GetActorLocation().Y));
	}	
}

void APlayerMovement::PanUp(float axis)
{
	if (GetActorLocation().X >= camMinX && GetActorLocation().X <= camMaxX)
	{
		FVector currLocation = GetActorLocation();
		currLocation += FVector::ForwardVector * axis * cameraVel * FApp::GetDeltaTime() * -1.0;
		if (currLocation.X > camMaxX)
		{
			currLocation.X = camMaxX;
		}
		else if (currLocation.X < camMinX)
		{
			currLocation.X = camMinX;
		}
		SetActorLocation(currLocation);
		GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::Printf(TEXT("Z Pos = %f"), GetActorLocation().X));
	}
}

void APlayerMovement::ZoomIn(float axis)
{
	if (GetActorLocation().Z >= camMinZ && GetActorLocation().Z <= camMaxZ)
	{
		FVector currLocation = GetActorLocation();
		currLocation += camera->GetForwardVector() * axis * cameraVel * FApp::GetDeltaTime() * -1.0;


		if ((axis > 0 && currLocation.Z < camMaxZ) || (axis < 0 && currLocation.Z > camMinZ))
		{
			SetActorLocation(currLocation);
		}

		GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::Printf(TEXT("Y Pos = %f"), GetActorLocation().Z));
	}
}

void APlayerMovement::AdjustTimeScale(float axis)
{
	ACapstoneProjectGameModeBase::timeScale += axis;
	ACapstoneProjectGameModeBase::timeScale = FMath::Clamp(ACapstoneProjectGameModeBase::timeScale, 0.f, 2.f);

	GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, FString::Printf(TEXT("TimeScale = %f"), ACapstoneProjectGameModeBase::timeScale));
}

void APlayerMovement::SpeedUpTime()
{
	AdjustTimeScale(timeScaleIncrement);
}

void APlayerMovement::SlowDownTime()
{
	AdjustTimeScale(-timeScaleIncrement);
}

