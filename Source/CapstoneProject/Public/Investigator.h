// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexNav.h"
#include "Investigator.generated.h"

UCLASS()
class CAPSTONEPROJECT_API AInvestigator : public AActor
{
	GENERATED_BODY()
	
public:	
	AInvestigator();
	virtual void Tick(float DeltaTime) override;

	void InitInvestigator(ABaseHex* hex, FUnitData* data);

private:
	void Investigate();
	void ClaimRewards();
	void EndInvestigation();

	UPROPERTY(EditAnywhere, Category = "Components") USceneComponent* root;
	UPROPERTY(EditAnywhere, Category = "Components") UHexNav* hexNav;
	UPROPERTY(EditAnywhere, Category = "Components") USkeletalMeshComponent* mesh;
	UPROPERTY(EditAnywhere, Category = "Components") UMeshVisibility* visibility;

	FUnitData* unit;
	FPointOfInterest* pointOfInterest;
};
