// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementAI.h"

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
}

// Called when the game starts or when spawned
void AMovementAI::BeginPlay()
{
	Super::BeginPlay();	
	currentHex = TestActor;
}

// Called every frame
void AMovementAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AMovementAI::CreatePath()
{
	if (hexPath.Num() > 0) hexPath.Empty();
	AActor* hexToSearch = currentHex;
	for (int i = 0; i < maxHexes; i++)
	{
		hexPath.Add(HexSearch(hexToSearch));
		
		if (hexPath[i] == targetHex) break;

		hexToSearch = hexPath[i];
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Path Created"));
}

void AMovementAI::SnapToHex(AActor* hex)
{
	SetActorLocation(hex->GetActorLocation());
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


