// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Faction.h"
#include "FactionEnum.h"
#include "BasePlayerController.h"
#include "CapstoneProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CAPSTONEPROJECT_API ACapstoneProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	ACapstoneProjectGameModeBase();
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	static inline TMap<Factions, Faction*> activeFactions;

	static inline float timeScale = 1.f;

	static inline float currentHarvestTime = 0;
	UPROPERTY(EditAnywhere) float harvestTickLength = 2.f;
	
	UPROPERTY(EditAnywhere) int alienFactionQuantity = 3;

	UPROPERTY(EditAnywhere) TSubclassOf<class AGlobalSpawner> spawner;

private:
	Factions CreateNewFaction();
	int factionCount = 0;
};
