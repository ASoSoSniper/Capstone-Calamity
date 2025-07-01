// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseHex.h"
#include "HexNav.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CAPSTONEPROJECT_API UHexNav : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHexNav();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	void SetCurrentHex(AActor* hex);
	void SetTargetHex(AActor* hex);
	void SetTargetHex(const ABaseHex* hex);
	ABaseHex* GetCurrentHex();
	const ABaseHex* GetTargetHex();

	bool CurrentEqualToTarget();

private:
	UPROPERTY(VisibleAnywhere) const ABaseHex* targetHex;

	UPROPERTY(VisibleAnywhere) ABaseHex* currentHex;

	bool movableEntity = false;
};
