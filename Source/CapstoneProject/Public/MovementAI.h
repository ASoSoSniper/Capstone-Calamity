// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "BaseHex.h"
#include "HexNav.h"
#include "UnitStats.h"
#include "MeshVisibility.h"
#include "Components/SphereComponent.h"
#include "ExploreAnims.h"
#include "MovementAI.generated.h"

UCLASS()
class CAPSTONEPROJECT_API AMovementAI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovementAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void CreatePath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere) UInteractable* interact;
	UPROPERTY(VisibleAnywhere) UHexNav* hexNav;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* mesh;
	UPROPERTY(EditAnywhere) UUnitStats* unitStats;
	UPROPERTY(EditAnywhere) UMeshVisibility* visibility;
	bool selectedByPlayer;

	void SetDestination(AActor* targetHex);
	void SetDestination(const ABaseHex* targetHex);
	void SphereCheck(float rangeMulti = 1.f);
	virtual void CancelPath();

protected:
	UPROPERTY(VisibleAnywhere) TArray<const ABaseHex*> hexPath;
	int hexPathIndex;
	UPROPERTY(VisibleAnywhere) float currTimeTillHexMove = 0.f;

	void CountdownToMove(float& DeltaTime);
	virtual void MoveToTarget(float& DeltaTime);
	
	virtual void Destroyed() override;

private:
	UPROPERTY(EditAnywhere) float traceStartOffset = 10.f;
	UPROPERTY(EditAnywhere) float traceLength = 50.f;
	UPROPERTY(EditAnywhere) float hexSearchDistance = 100.f;
	UPROPERTY(EditAnywhere) float hexSnapDistance = 2.f;
	UPROPERTY(EditAnywhere) UExploreAnims* anims;
	UPROPERTY(EditAnywhere) int maxHexes = 200;
	UPROPERTY(EditAnywhere) float moveSpeed = 1.f;
	float currentMoveAlpha = 0.f;

	float AngleBetweenVectors(FVector a, FVector b);
	FVector GetVectorToTarget(FVector origin);
	bool HexIsTraversable(AActor* hex);
	bool HexIsTraversable(const ABaseHex* hex);
	void SnapToHex(ABaseHex* hex);
	ABaseHex* HexSearch(const ABaseHex* hex);

	struct FNodeData
	{
	public:
		const ABaseHex* hex;
		const FNodeData* parent;
		const int x;
		const int y;

		inline float GetF() const;
		inline float GetG() const;
		inline float GetH() const;

		FNodeData() : hex(nullptr), parent(nullptr), x(0), y(0){}
		FNodeData(const ABaseHex* setHex, const FNodeData* setParent, int setX, int setY, const FVector2D destination = FVector2D::Zero()) : hex(setHex), parent(setParent), x(setX), y(setY)
		{
			g = setParent ? (setParent->GetG() - 1) : 0;
			h = FMath::Sqrt(
				FMath::Abs((float)FMath::Square(destination.X - (float)setX) + 
				FMath::Abs((float)FMath::Square(destination.Y - (float)setY))));
		}

	private:
		float g = 0;
		float h = 0;
	};

	TArray<const ABaseHex*> GeneratePath_Legacy(const ABaseHex* destination, const ABaseHex* prevStep = nullptr);
	TArray<const ABaseHex*> GeneratePath_AStar(const ABaseHex* destination, const ABaseHex* prevStep = nullptr);

	enum MoveStates
	{
		Idle,
		Move,
		Attack
	};
	MoveStates moveState = Idle;
};
