// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovement.h"
#include "CapstoneProjectGameModeBase.h"
#include "BasePlayerController.h"

// Sets default values
APlayerMovement::APlayerMovement()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(RootComponent);
	camera->SetWorldRotation(FRotator(-45.f, 0.f, 0.f));

	audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	audioComponent->SetupAttachment(RootComponent);
	audioComponent->SetUISound(true);

	movement = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Movement"));
}

// Called when the game starts or when spawned
void APlayerMovement::BeginPlay()
{
	Super::BeginPlay();
	
	AController* tempController = GetController();
	controller = Cast<ABasePlayerController>(tempController);

	if (!soundBoxPrefab) soundBoxPrefab = ASoundBox::StaticClass();
}

// Called every frame
void APlayerMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HexCast();
	DragCamera();

	if (controlState == Cinematic)
		MoveToCinematicPos(DeltaTime);
}

// Called to bind functionality to input
void APlayerMovement::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerMovement::PanRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerMovement::PanUp);
	PlayerInputComponent->BindAxis("ZoomIn", this, &APlayerMovement::ZoomIn);
	PlayerInputComponent->BindAxis("MouseX", this, &APlayerMovement::GetMouseX);
	PlayerInputComponent->BindAxis("MouseY", this, &APlayerMovement::GetMouseY);

	//PlayerInputComponent->BindAction("Deselect", IE_Pressed, this, &APlayerMovement::DeselectInput);
	PlayerInputComponent->BindAction("SelectAction", IE_Pressed, this, &APlayerMovement::SelectInput);
	PlayerInputComponent->BindAction("SelectAction", IE_Released, this, &APlayerMovement::StartSelect);
	PlayerInputComponent->BindAction("CommandAction", IE_Pressed, this, &APlayerMovement::StartCommand);

	PlayerInputComponent->BindAction("Action9", IE_Pressed, this, &APlayerMovement::Action9Input);
	PlayerInputComponent->BindAction("Action10", IE_Pressed, this, &APlayerMovement::Action10Input);
	PlayerInputComponent->BindAction("Action11", IE_Pressed, this, &APlayerMovement::Action11Input);
	PlayerInputComponent->BindAction("Action12", IE_Pressed, this, &APlayerMovement::Action12Input);
}

void APlayerMovement::MoveToCinematicPos(float& DeltaTime)
{
	if (controlState != Cinematic) return;

	if (currCinemaTime < 1)
	{
		currCinemaTime = FMath::Clamp(currCinemaTime + (DeltaTime / targetCinemaPos.travelTime), 0, 1);
		FVector step = FMath::Lerp(startPos, targetCinemaPos.position, currCinemaTime);

		SetActorLocation(step);
		return;
	}

	if (currLingerTime < 1)
	{
		currLingerTime += DeltaTime / targetCinemaPos.lingerTime;
		return;
	}

	ACapstoneProjectGameModeBase::gameState = gameState;

	controlState = Free;
}

void APlayerMovement::Action1Input()
{
	if (controlState != Free) return;
}

void APlayerMovement::Action2Input()
{
	if (controlState != Free) return;

	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action2();
	}
}

void APlayerMovement::Action3Input()
{
	if (controlState != Free) return;

	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action3();
	}
}

void APlayerMovement::Action4Input()
{
	if (controlState != Free) return;

	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action4();
	}
}

void APlayerMovement::Action5Input()
{
	if (controlState != Free) return;

	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action5();
	}
}

void APlayerMovement::Action6Input()
{
	if (controlState != Free) return;

	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action6();
	}
}

void APlayerMovement::Action7Input()
{
	if (controlState != Free) return;

	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action7();
	}
}

void APlayerMovement::Action8Input()
{
	if (controlState != Free) return;

	if (controller)
	{
		controller->actionStates[controller->currentActionState]->Action8();
	}
}

void APlayerMovement::Action9Input()
{
	UnitActions::GetFaction(Factions::Alien1)->LowerHostility(controller->playerFaction, 1.f);
}

void APlayerMovement::Action10Input()
{
	UnitActions::GetFaction(Factions::Alien1)->IncreaseHostility(controller->playerFaction, 1.f);
}

void APlayerMovement::Action11Input()
{
	UnitActions::GetFaction(Factions::Alien2)->LowerHostility(controller->playerFaction, 1.f);
}

void APlayerMovement::Action12Input()
{
	UnitActions::GetFaction(Factions::Alien2)->IncreaseHostility(controller->playerFaction, 1.f);
}

void APlayerMovement::DeselectInput()
{
	if (controlState != Free) return;

	if (controller) controller->Deselect();
}

void APlayerMovement::SelectInput()
{
	if (controlState != Free) return;

	clickHeld = true;

	float x;
	float y;
	controller->GetMousePosition(x, y);
	previousMouseLocation = FVector2D(x, y);

	if (controller) controller->activeSelecting = true;
}

void APlayerMovement::StartSelect()
{
	if (controlState != Free) return;

	clickHeld = false;
	
	if (controller)
	{
		if (controller->hoveredWorldObject && controller->activeSelecting) 
			controller->SetSelectedWorldObject(controller->hoveredWorldObject);

		controller->hoveredWorldObject = nullptr;
		controller->activeSelecting = false;
	}
}

void APlayerMovement::StartCommand()
{
	if (controlState != Free) return;

	if (controller) controller->CommandAction();
}

void APlayerMovement::GetMouseX(float axis)
{
	mouseX = axis;
}

void APlayerMovement::GetMouseY(float axis)
{
	mouseY = axis;
}

void APlayerMovement::DragCamera()
{
	if (controlState != Free) return;
	if (!clickHeld) return;

	float x;
	float y;
	controller->GetMousePosition(x, y);
	currentMouseLocation = FVector2D(x, y);

	FVector2D direction = currentMouseLocation - previousMouseLocation;
	if (direction.Size() > 1.f) controller->activeSelecting = false;

	FVector step = FVector(-direction.Y, direction.X, 0);

	AddActorWorldOffset(step);
	previousMouseLocation = currentMouseLocation;
}

void APlayerMovement::PanRight(float axis)
{
	if (controlState != Free) return;

	if (axis < 0)
	{
		if (GetActorLocation().Y < camMaxY)
		{
			FVector currLocation = GetActorLocation();

			currLocation += FVector::RightVector * axis * cameraVel * FApp::GetDeltaTime() * -1.0;

			if (currLocation.Y <= camMaxY)
			{
				SetActorLocation(currLocation);
			}
		}
	}
	else if (axis > 0)
	{
		if (GetActorLocation().Y > camMinY)
		{
			FVector currLocation = GetActorLocation();

			currLocation += FVector::RightVector * axis * cameraVel * FApp::GetDeltaTime() * -1.0;

			if (currLocation.Y >= camMinY)
			{
				SetActorLocation(currLocation);
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::Printf(TEXT("Y pos = %f"), GetActorLocation().Y));

}

void APlayerMovement::PanUp(float axis)
{
	if (controlState != Free) return;

	if (axis < 0)
	{
		if (GetActorLocation().X < camMaxX + GetActorLocation().Z)
		{
			FVector currLocation = GetActorLocation();
			
			currLocation += FVector::ForwardVector * axis * cameraVel * FApp::GetDeltaTime() * -1.0;
			
			if (currLocation.X <= camMaxX + GetActorLocation().Z)
			{
				SetActorLocation(currLocation);
			}	
		}
	}
	else if (axis > 0)
	{
		if (GetActorLocation().X > camMinX + GetActorLocation().Z)
		{
			FVector currLocation = GetActorLocation();

			currLocation += FVector::ForwardVector * axis * cameraVel * FApp::GetDeltaTime() * -1.0;

			if (currLocation.X >= camMinX + GetActorLocation().Z)
			{
				SetActorLocation(currLocation);
			}		
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::Printf(TEXT("X Pos = %f"), GetActorLocation().X));
}

void APlayerMovement::ZoomIn(float axis)
{
	if (controlState != Free) return;

	if (GetActorLocation().Z >= camMinZ && GetActorLocation().Z <= camMaxZ)
	{
		FVector currLocation = GetActorLocation();
		currLocation += camera->GetForwardVector() * axis * cameraVel * FApp::GetDeltaTime();
		
		if (currLocation.Z > camMaxZ)
		{
			currLocation.Z = camMaxZ;
		}
		else if (currLocation.Z < camMinZ)
		{
			currLocation.Z = camMinZ;
		}

		if (currLocation.Z < camMaxZ && currLocation.Z > camMinZ)
		{
			SetActorLocation(currLocation);
		}

		//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, FString::Printf(TEXT("Z Pos = %f"), GetActorLocation().Z));
	}
}

void APlayerMovement::AdjustTimeScale(float axis)
{
	if (controlState != Free) return;

	ACapstoneProjectGameModeBase::timeScale += axis;
	ACapstoneProjectGameModeBase::timeScale = FMath::Clamp(ACapstoneProjectGameModeBase::timeScale, 0.f, 2.f);

	//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, FString::Printf(TEXT("TimeScale = %f"), ACapstoneProjectGameModeBase::timeScale));
}

void APlayerMovement::HexCast()
{
	TArray<FHitResult> results;
	FVector start = GetActorLocation();
	FVector end = start + camera->GetForwardVector() * 10000.f;

	bool hit = GetWorld()->LineTraceMultiByChannel(results, start, end, ECC_Visibility);
	if (!hit) return;

	ABaseHex* foundHex = nullptr;
	int32 index = 0;
	for (int i = 0; i < results.Num(); i++)
	{
		foundHex = Cast<ABaseHex>(results[i].GetActor());
		if (foundHex)
		{
			index = i;
			break;
		}
	}

	if (!foundHex) return;

	if (!soundBox)
	{
		soundBox = Cast<ASoundBox>(GetWorld()->SpawnActor(soundBoxPrefab));
	}

	FVector boxLocation = FVector(results[index].ImpactPoint.X, results[index].ImpactPoint.Y, foundHex->GetActorLocation().Z);
	soundBox->SetActorLocation(boxLocation);

	bool canPlayEnvSounds = GetActorLocation().Z <= soundBox->cameraZoomToTrigger;
	soundBox->ToggleSoundsActive(canPlayEnvSounds);
}

