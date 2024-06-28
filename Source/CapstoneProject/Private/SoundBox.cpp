// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundBox.h"
#include "Building.h"
#include "MeshVisibility.h"

// Sets default values
ASoundBox::ASoundBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	collider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	collider->OnComponentBeginOverlap.AddDynamic(this, &ASoundBox::OnOverlapBegin);
	collider->OnComponentEndOverlap.AddDynamic(this, &ASoundBox::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ASoundBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoundBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AdjustHexVolumes();
}

void ASoundBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseHex* hex = Cast<ABaseHex>(OtherActor))
	{
		if (!foundHexes.Contains(hex)) foundHexes.Add(hex);
		hex->SetInSoundBoxRadius(true);
		SetHexAmbience(hex);
	}
}

void ASoundBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseHex* hex = Cast<ABaseHex>(OtherActor))
	{
		foundHexes.Remove(hex);
		hex->SetInSoundBoxRadius(false);
		hex->SetTargetVolume(0.f);
	}
}

void ASoundBox::AssignHexes()
{
	if (!soundsActive) return;

	for (int i = 0; i < foundHexes.Num(); i++)
	{
		//SetHexAmbience(foundHexes[i]);
	}
}

void ASoundBox::SetHexAmbience(ABaseHex* hex)
{
	if (!hex->visibility->factionVisibility[Factions::Human].discoveredByFaction) return;

	bool soundSet = false;
	
	if (hex->battle)
	{
		if (!soundSet) hex->audioComponent->SetSound(battleSound);
		soundSet = true;
	}

	if (hex->building)
	{
		if (buildingSounds.Contains(hex->building->GetBuildingType()))
		{
			if (!soundSet) hex->audioComponent->SetSound(buildingSounds[hex->building->GetBuildingType()]);
			soundSet = true;
		}
	}

	if (terrainSounds.Contains(hex->GetHexTerrain()))
	{
		if (!soundSet) hex->audioComponent->SetSound(terrainSounds[hex->GetHexTerrain()]);
		soundSet = true;
	}

	if (!soundSet) return;
	hex->audioComponent->Play();
}

void ASoundBox::AdjustHexVolumes()
{
	if (!soundsActive)
	{
		for (int i = 0; i < foundHexes.Num(); i++)
		{
			foundHexes[i]->SetTargetVolume(0.f);
			return;
		}
	}

	FVector center = GetActorLocation();
	for (int i = 0; i < foundHexes.Num(); i++)
	{
		FVector hexPos = foundHexes[i]->GetActorLocation();
		FVector flatPos = FVector(hexPos.X, hexPos.Y, center.Z);

		float distanceToCenter = FVector::Distance(center, flatPos);

		float radius = collider->GetUnscaledSphereRadius();

		float volume = FMath::Clamp(((minVolume - maxVolume) / radius) * distanceToCenter + maxVolume, minVolume, maxVolume);

		foundHexes[i]->SetTargetVolume(volume);
	}
}

void ASoundBox::ToggleSoundsActive(bool active)
{
	if (soundsActive == active) return;

	soundsActive = active;

	if (soundsActive)
	{
		AssignHexes();
	}
	else
	{
		for (int i = 0; i < foundHexes.Num(); i++)
		{
			//foundHexes[i]->audioComponent->FadeOut(fadeOutSpeed, 0.f);
			foundHexes[i]->SetTargetVolume(0.f);
		}
	}
}

