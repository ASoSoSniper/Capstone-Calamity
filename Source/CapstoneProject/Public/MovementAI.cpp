// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementAI.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "CapstoneProjectGameModeBase.h"

// Sets default values
AMovementAI::AMovementAI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	interact = CreateDefaultSubobject<UInteractable>(TEXT("Interaction Component"));
	hexNav = CreateDefaultSubobject<UHexNav>(TEXT("Hex Nav"));
	unitStats = CreateDefaultSubobject<UUnitStats>("Faction Stats");
	visibility = CreateDefaultSubobject<UMeshVisibility>("Mesh Visibility");
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = mesh;

	USkeletalMesh* meshComponent = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));
	if (meshComponent)
	{
		mesh->SetSkeletalMesh(meshComponent);
	}
	mesh->SetCollisionProfileName("BlockAllDynamic");

	mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	
}

// Called when the game starts or when spawned
void AMovementAI::BeginPlay()
{
	Super::BeginPlay();	

	selectedByPlayer = false;

	anims = Cast<UExploreAnims>(mesh->GetAnimInstance());

	SphereCheck(20.f);
}

// Called every frame
void AMovementAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (anims) anims->timeScale = ACapstoneProjectGameModeBase::timeScale;
	
	if (!hexNav->GetCurrentHex())
	{
		SphereCheck(20.f);
		return;
	}

	switch (moveState)
	{
	case Idle:
		break;
	case Move:
		SphereCheck(ACapstoneProjectGameModeBase::timeScale);

		CountdownToMove(DeltaTime);
		
		MoveToTarget(DeltaTime);
		break;
	}
}

void AMovementAI::CreatePath()
{
	if (!HexIsTraversable(hexNav->GetTargetHex()))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Cannot move here"));
		return;
	}

	AActor* prevDestination = (!hexPath.IsEmpty() && moveState == Move) ? hexPath[hexPathIndex] : nullptr;

	//Continue moving toward destination if new hex path is created while moving toward previous destination
	AActor* temp = nullptr;
	if (!hexPath.IsEmpty() && currTimeTillHexMove >= unitStats->speed)
	{
		temp = hexPath[hexPathIndex];
	}

	//Reset path data
	hexPathIndex = 0;
	if (hexPath.Num() > 0) hexPath.Empty();

	//Add last hex destination to new path
	if (temp != nullptr)
		hexPath.Add(temp);

	if (temp != hexNav->GetTargetHex())
	{
		//Choose initial hex
		AActor* hexToSearch = (temp == nullptr) ? hexNav->GetCurrentHex() : temp;

		//Scan for hexes leading to the target hex	
		for (int i = 0; i < maxHexes; i++)
		{
			hexPath.Add(HexSearch(hexToSearch));

			if (hexPath[i] == hexNav->GetTargetHex()) break;

			hexToSearch = hexPath[i];
		}
	}

	//Begin moving along path
	moveState = Move;

	if (anims)
	{
		anims->isWalking = true;
	}

	if ((currTimeTillHexMove < unitStats->speed) && (hexPath[0] != prevDestination))
	{
		currTimeTillHexMove = 0.f;
	}
}

void AMovementAI::SetDestination(AActor* targetHex)
{
	hexNav->SetTargetHex(targetHex);

	//If the new target hex equals the current hex or the destination it's already moving toward, do not create a new path
	if (hexNav->CurrentEqualToTarget()) return;
	if ((!hexPath.IsEmpty() && moveState == Move && hexNav->GetTargetHex() == hexPath[hexPath.Num() - 1])) return;

	//Otherwise, begin path creation
	CreatePath();
}

void AMovementAI::SnapToHex(ABaseHex* hex)
{
	//Snap position to hex
	SetActorLocation(hex->troopAnchor->GetComponentLocation());

	//Remove this unit from previous hex troop pool
	ABaseHex* previousHex = hexNav->GetCurrentHex();
	if (previousHex)
	{
		previousHex->RemoveTroopFromHex(this);
	}

	//Add this unit to the current hex troop pool
	hex->AddTroopToHex(this);
}

ABaseHex* AMovementAI::HexSearch(AActor* hex)
{	
	if (!Cast<ABaseHex>(hex))
	{
		return nullptr;
	}

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(hex);
	TArray<AActor*> objectsInHex = Cast<ABaseHex>(hex)->GetObjectsInHex();
	if (objectsInHex.Num() > 0) queryParams.AddIgnoredActors(objectsInHex);

	TArray<ABaseHex*> hexesFound;
	TArray<float> anglesToTarget;

	//For each of the 6 directions:
	for (int i = 0; i < 6; i++)
	{
		//Establish trace direction
		FVector traceStart = hex->GetActorLocation() + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceStartOffset;
		FVector traceEnd = traceStart + FRotationMatrix(FRotator(0, 30 + (i * 60), 0)).GetUnitAxis(EAxis::X) * traceLength;
		FHitResult hit;

		//Trace
		bool found = GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams, FCollisionResponseParams::DefaultResponseParam);

		//Check for hex actor class
		ABaseHex* foundHex = found ? Cast<ABaseHex>(hit.GetActor()) : nullptr;

		if (hexPath.Contains(foundHex))
		{
			foundHex = nullptr;
		}

		//Identify found hex
		hexesFound.Add(foundHex ? foundHex : nullptr);

		//Check if hex is traversable
		bool traversable = true;
		if (foundHex) traversable = HexIsTraversable(foundHex);

		//Get hex's angle to target
		if (hexNav->GetTargetHex())
		{
			float angle = INFINITY;
			if (foundHex && traversable)
			{
				angle = AngleBetweenVectors(traceEnd - traceStart, GetVectorToTarget(hex->GetActorLocation()));
			}
			anglesToTarget.Add(angle);
		}	

		// **Debug things**
		//DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Red, false, 1.f);
	}

	ABaseHex* closestHexToTarget = nullptr;
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

//Unfinished, research A-Star search algorithm to implement
TArray<AActor*> AMovementAI::GeneratePath(ABaseHex* destination)
{
	TArray<AActor*> path;

	TQueue<ABaseHex*> hexesToSearch;
	TSet<ABaseHex*> searchedHexes;

	if (AGlobalSpawner::spawnerObject) return path;

	int dir[6][2] = { {1,0}, {-1,0}, {0,1}, {0,-1}, {1,1}, {-1,1} };

	hexesToSearch.Enqueue(hexNav->GetCurrentHex());

	int max = AGlobalSpawner::spawnerObject->hexArray.Num() - 1;

	while (!hexesToSearch.IsEmpty())
	{
		ABaseHex* currHex = nullptr;
		if (!hexesToSearch.Dequeue(currHex)) break;
		searchedHexes.Add(currHex);

		FVector2D currCell = AGlobalSpawner::spawnerObject->GetHexCoordinates(currHex);

		for (int i = 0; i < 6; i++)
		{
			int x = currCell[0] + dir[i][0];
			int y = currCell[1] + dir[i][1];

			if (x < 0 || x > max || y < 0 || y > max) continue;

			ABaseHex* hex = AGlobalSpawner::spawnerObject->hexArray[x][y];
			if (searchedHexes.Contains(hex)) continue;


		}
	}

	return path;
}

void AMovementAI::SphereCheck(float rangeMulti)
{
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	TArray<FHitResult> results;
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetActorLocation(), GetActorLocation(), hexSearchDistance, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::None, results, true);

	if (bHit)
	{		
		for (int i = 0; i < results.Num(); i++)
		{
			ABaseHex* hexActor = Cast<ABaseHex>(results[i].GetActor());
			if (hexActor && results[i].GetActor() != hexNav->GetCurrentHex())
			{
				if (FMath::Abs(GetActorLocation().X - hexActor->GetActorLocation().X) < hexSnapDistance * rangeMulti && FMath::Abs(GetActorLocation().Y - hexActor->GetActorLocation().Y) < hexSnapDistance * rangeMulti)
				{
					SnapToHex(hexActor);
					//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Snapped to target"));
					break;
				}
			}
		}
	}
}

bool AMovementAI::HexIsTraversable(AActor* hex)
{
	ABaseHex* foundHex = Cast<ABaseHex>(hex);
	return foundHex->IsTraversableTerrain();
}

bool AMovementAI::HexIsTraversable(ABaseHex* hex)
{
	return hex->IsTraversableTerrain();
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
	FVector targetDirection = hexNav->GetTargetHex()->GetActorLocation() - origin;
	return targetDirection;
}

void AMovementAI::CountdownToMove(float& DeltaTime)
{
	ABaseHex* targetHex = Cast<ABaseHex>(hexPath[hexPathIndex]);
	if (!targetHex) return;

	currTimeTillHexMove += DeltaTime * targetHex->GetMovementMulti() * ACapstoneProjectGameModeBase::timeScale;
	currTimeTillHexMove = FMath::Clamp(currTimeTillHexMove, 0.f, unitStats->speed);
}

void AMovementAI::MoveToTarget(float& DeltaTime)
{
	if (!hexNav->GetCurrentHex())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, TEXT("No current hex"));
		return;
	}

	if (currTimeTillHexMove < unitStats->speed)
	{
		return;
	}

	if (hexPath.Num() > 0)
	{
		ABaseHex* hex = hexNav->GetCurrentHex();
		ABaseHex* targetHex = Cast<ABaseHex>(hexPath[hexPathIndex]);

		currentMoveAlpha += moveSpeed * DeltaTime * ACapstoneProjectGameModeBase::timeScale;
		currentMoveAlpha = FMath::Clamp(currentMoveAlpha, 0.f, 1.f);

		FVector step = FMath::Lerp(hex->troopAnchor->GetComponentLocation(), targetHex->troopAnchor->GetComponentLocation(), currentMoveAlpha);
		SetActorLocation(step);

		if (hex == hexPath[hexPathIndex])
		{
			hexPathIndex++;
			currentMoveAlpha = 0.f;
			currTimeTillHexMove = 0.f;
			
			if (hexPathIndex > hexPath.Num() - 1)
			{
				moveState = Idle;

				if (anims) anims->isWalking = false;
				return;
			}
		}
	}
}

void AMovementAI::CancelPath()
{
	if (hexPath.IsEmpty() || hexNav->CurrentEqualToTarget()) return;

	if (currTimeTillHexMove >= unitStats->speed)
	{
		SetDestination(hexPath[hexPathIndex]);
	}
	else
	{
		moveState = Idle;
		currTimeTillHexMove = 0.f;
	}
}

void AMovementAI::Destroyed()
{
	if (hexNav->GetCurrentHex())
	{
		UnitActions::RemoveFromFaction(unitStats->faction, this);
		hexNav->GetCurrentHex()->troopsInHex.Remove(this);

		UnitActions::RemoveArmyName(Factions::Human, this);

		if (selectedByPlayer)
		{
			AActor* controllerTemp = UGameplayStatics::GetActorOfClass(GetWorld(), ABasePlayerController::StaticClass());
			ABasePlayerController* controller = Cast<ABasePlayerController>(controllerTemp);

			if (controller) controller->Deselect();
		}
	}

	Super::Destroyed();
}


