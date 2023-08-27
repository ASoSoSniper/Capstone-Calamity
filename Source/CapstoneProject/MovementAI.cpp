// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementAI.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMovementAI::AMovementAI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMovementAI::BeginPlay()
{
	Super::BeginPlay();
	HexSearch(TestActor);
}

// Called every frame
void AMovementAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMovementAI::SnapToHex(AActor* hex)
{
	SetActorLocation(hex->GetActorLocation());
}

void AMovementAI::HexSearch(AActor* hex)
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(hex);

	//TArray<bool, float> angles;

	for (int i = 0; i < 6; i++)
	{
		FVector traceStart = hex->GetActorLocation() + FRotationMatrix(FRotator(0, i * 60, 0)).GetUnitAxis(EAxis::X) * traceStartOffset;
		FVector traceEnd = traceStart + FRotationMatrix(FRotator(0, i * 60, 0)).GetUnitAxis(EAxis::X) * traceLength;
		FHitResult hit;

		bool foundHex = GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams, FCollisionResponseParams::DefaultResponseParam);
		//angles[i] = foundHex, AngleBetweenVectors(FVector::UpVector, FVector::RightVector);

		//Debug things
		DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Red, true);
		if (foundHex)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Object found!"));
		}
	}
}

float AMovementAI::AngleBetweenVectors(const FVector& a, const FVector& b)
{
	//Find Dot product of a and b
	float dot = FVector::DotProduct(a.GetSafeNormal(), b.GetSafeNormal());

	//Find the absolute value of the acos of the dot, converted to degrees
	float angle = FMath::RadiansToDegrees(FMath::Abs(acos(dot)));

	//Return value
	return angle;
}

FVector AMovementAI::GetVectorToTarget(FVector& origin)
{

	return FVector();
}

