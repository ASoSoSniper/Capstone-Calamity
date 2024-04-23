// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicObject.h"

// Sets default values
AMusicObject::AMusicObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));

	audio->bAllowSpatialization = false;
}

// Called when the game starts or when spawned
void AMusicObject::BeginPlay()
{
	Super::BeginPlay();
	
	if (!music.IsEmpty())
	{
		audio->SetSound(music[0]);
		audio->Play();
	}
}

// Called every frame
void AMusicObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (audio->IsPlaying())
	{
		currentTime = timeBetweenSongs;
		return;
	}

	currentTime -= DeltaTime;
	if (currentTime <= 0.f)
	{
		musicIndex++;
		if (!music.IsValidIndex(musicIndex)) musicIndex = 0;

		audio->SetSound(music[musicIndex]);
		audio->Play();
	}
}

