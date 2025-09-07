// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UAI_Controller.h"
#include "FactionController.generated.h"

UCLASS()
class CAPSTONEPROJECT_API AFactionController : public AActor, public IUAI_Controller
{
	GENERATED_BODY()
	
public:	
	AFactionController();
	virtual void Tick(float DeltaTime) override;

	void SetFaction(Faction* setFaction);
	virtual bool IsAIControlled() override;

protected:
	virtual void BeginPlay() override;
	
	virtual float GetUpdateRate() const override;

private:
	Faction* faction;

	void DateUpdateTick();

	UPROPERTY(EditAnywhere, Category = "Actions") TMap<EActionType, FActionSelection> troopActions;
	UPROPERTY(EditAnywhere, Category = "Actions") TMap<EActionType, FActionSelection> buildingActions;
};
