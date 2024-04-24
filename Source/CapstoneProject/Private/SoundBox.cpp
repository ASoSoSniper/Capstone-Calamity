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
		hex->inSoundboxRadius = true;
		SetHexAmbience(hex);
	}
}

void ASoundBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseHex* hex = Cast<ABaseHex>(OtherActor))
	{
		foundHexes.Remove(hex);
		hex->inSoundboxRadius = false;
		hex->targetVolume = 0.f;
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
	
	if (hex->battleInProgress)
	{
		if (!soundSet) hex->audioComponent->SetSound(battleSound);
		soundSet = true;
	}

	if (hex->building)
	{
		if (buildingSounds.Contains(hex->building->buildingType))
		{
			if (!soundSet) hex->audioComponent->SetSound(buildingSounds[hex->building->buildingType]);
			soundSet = true;
		}
	}

	if (terrainSounds.Contains(hex->hexTerrain))
	{
		if (!soundSet) hex->audioComponent->SetSound(terrainSounds[hex->hexTerrain]);
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
			foundHexes[i]->targetVolume = 0.f;
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

		foundHexes[i]->targetVolume = volume;
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
			foundHexes[i]->targetVolume = 0.f;
		}
	}
}

