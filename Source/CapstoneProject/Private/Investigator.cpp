// Fill out your copyright notice in the Description page of Project Settings.


#include "Investigator.h"
#include "GlobalSpawner.h"
#include "Faction.h"

// Sets default values
AInvestigator::AInvestigator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	hexNav = CreateDefaultSubobject<UHexNav>(TEXT("Hex Nav"));
	visibility = CreateDefaultSubobject<UMeshVisibility>(TEXT("Visibility"));

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	mesh->SetupAttachment(root);

	USkeletalMesh* meshComponent = LoadObject<USkeletalMesh>(nullptr, TEXT("StaticMesh '/Game/3DModels/Animations/robot_all_beta.robot_all_beta'"));
	if (meshComponent)
	{
		mesh->SetSkeletalMesh(meshComponent);
	}

	mesh->SetCollisionProfileName("NoCollision");
	mesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

// Called every frame
void AInvestigator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Investigate();
}

void AInvestigator::InitInvestigator(ABaseHex* hex, FUnitData* data)
{
	unit = data;
	hexNav->SetCurrentHex(hex);

	if (!hex || !hex->HasPOI()) EndInvestigation();

	visibility->faction = unit->GetFaction();
	pointOfInterest = hex->GetPOI();

	SetActorLocation(hex->troopAnchor->GetComponentLocation());
}

void AInvestigator::Investigate()
{
	if (!pointOfInterest) EndInvestigation();

	if (pointOfInterest->Work()) ClaimRewards();
}

void AInvestigator::ClaimRewards()
{
	if (UFaction* f = UnitActions::GetFaction(unit->GetFaction()))
	{
		for (const TPair<EStratResources, int> r : pointOfInterest->GetRewards())
		{
			f->CollectResource(r.Key, r.Value);
		}
	}
	EndInvestigation();
}

void AInvestigator::EndInvestigation()
{
	ATroop* spawn = AGlobalSpawner::spawnerObject->SpawnArmyByUnit(hexNav->GetCurrentHex(), unit);

	hexNav->GetCurrentHex()->EndPOIInvestigation();
	Destroy();
}

