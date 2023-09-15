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

	static inline TMap<Factions, Faction*> activeFactions;
	
	UPROPERTY(EditAnywhere) int alienFactionQuantity = 3;


private:
	Factions CreateNewFaction();
	int factionCount = 0;
};
