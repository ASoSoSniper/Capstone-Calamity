// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementAI.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMovementAI::AMovementAI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	interact = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));
	sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Body"));
	sphere->SetupAttachment(RootComponent);
	sphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	sphere->InitSphereRadius(20.f);
}

// Called when the game starts or when spawned
void AMovementAI::BeginPlay()
{
	Super::BeginPlay();	

	SphereCheck();
}

// Called every frame
void AMovementAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	switch (moveState)
	{
	case Idle:
		break;
	case Move:
		SphereCheck();
		MoveToTarget(DeltaTime);
		break;
	}
}

void AMovementAI::CreatePath()
{
	if (targetHex == currentHex) return;

	//Save last hex destination, if interrupted
	AActor* temp = nullptr;
	if (moveState == Move)
	{
		temp = hexPath[hexPathIndex];
	}

	//Reset path data
	hexPathIndex = 0;
	if (hexPath.Num() > 0) hexPath.Empty();

	//Add last hex destination to new path
	if (temp != nullptr)
		hexPath.Add(temp);

	//Choose initial hex
	AActor* hexToSearch = (temp == nullptr) ? currentHex : temp;

	//Scan for hexes leading to the target hex	
	for (int i = 0; i < maxHexes; i++)
	{
		hexPath.Add(HexSearch(hexToSearch));
		
		if (hexPath[i] == targetHex) break;

		hexToSearch = hexPath[i];
	}

	//Begin moving along path
	moveState = Move;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Path Created"));
}

void AMovementAI::SnapToHex(AActor* hex)
{
	SetActorLocation(hex->GetActorLocation() + FVector::UpVector * 25);
	currentHex = hex;
}

AActor* AMovementAI::HexSearch(AActor* hex)
{	
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(hex);

	TArray<AActor*> hexesFound;
	TArray<float> anglesToTarget;

	//For each of the 6 directions:
	for (int i = 0; i < 6; i++)
	{
		//Establish trace direction
		FVector traceStart = hex->GetActorLocation() + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceStartOffset;
		FVector traceEnd = traceStart + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceLength;
		FHitResult hit;

		//Trace
		bool foundHex = GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams, FCollisionResponseParams::DefaultResponseParam);

		//Identify found hex
		hexesFound.Add(foundHex ? hit.GetActor() : nullptr);

		//Get hex's angle to target
		if (targetHex)
		{
			anglesToTarget.Add(foundHex ? AngleBetweenVectors(traceEnd - traceStart, GetVectorToTarget(hex->GetActorLocation())) : INFINITY);
		}	

		// **Debug things**
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Red, false, 1.f);
	}

	AActor* closestHexToTarget = nullptr;
	float smallestAngle = INFINITY;
	for (int i = 0; i < 6; i++)
	{
		if (anglesToTarget[i] < smallestAngle)
		{
			smallestAngle = anglesToTarget[i];
			closestHexToTarget = hexesFound[i];
		}
	}

	return closestHexToTarget;
}

void AMovementAI::SphereCheck()
{
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	TArray<FHitResult> results;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetActorLocation(), GetActorLocation(), 20.f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::ForOneFrame, results, true);

	if (bHit)
	{
		
		for (int i = 0; i < results.Num(); i++)
		{
			if (results[i].GetActor()->FindComponentByClass<UHexInfo>() && results[i].GetActor() != currentHex)
			{
				if (FMath::Abs(GetActorLocation().X - results[i].GetActor()->GetActorLocation().X) < hexSnapDistance && FMath::Abs(GetActorLocation().Y - results[i].GetActor()->GetActorLocation().Y) < hexSnapDistance)
				{
					SnapToHex(results[i].GetActor());
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Snapped to target"));
					break;
				}
			}
		}
	}
}

float AMovementAI::AngleBetweenVectors(FVector a, FVector b)
{
	//Find Dot product of a and b
	float dot = FVector::DotProduct(a.GetSafeNormal(), b.GetSafeNormal());

	//Find the absolute value of the acos of the dot, converted to degrees
	float angle = FMath::RadiansToDegrees(FMath::Abs(acos(dot)));

	//Return value
	return angle;
}

FVector AMovementAI::GetVectorToTarget(FVector origin)
{
	FVector targetDirection = targetHex->GetActorLocation() - origin;
	return targetDirection;
}

void AMovementAI::MoveToTarget(float& DeltaTime)
{
	if (hexPath.Num() > 0)
	{
		FVector direction = hexPath[hexPathIndex]->GetActorLocation() - currentHex->GetActorLocation();
		FVector newLocation = GetActorLocation() + direction * moveSpeed * DeltaTime;
		SetActorLocation(newLocation);
		if (currentHex == hexPath[hexPathIndex])
		{
			hexPathIndex++;
			if (hexPathIndex > hexPath.Num() - 1)
			{
				moveState = Idle;
			}
		}
	}
}


