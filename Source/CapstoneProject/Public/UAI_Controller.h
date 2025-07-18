// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UAI_Controller.generated.h"

class UAI_Action;
class UAI_Condition;

UENUM(BlueprintType)
enum class EActionType : uint8
{
	None,
	Explore,
	Attack
};

UENUM(BlueprintType)
enum class EDecisionState : uint8
{
	Deciding,
	Moving,
	Executing
};

USTRUCT(BlueprintType)
struct FActionSelection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere) FString displayName;
	UPROPERTY(EditAnywhere) TArray<UAI_Action*> actions;
	UPROPERTY(EditAnywhere) TArray<UAI_Condition*> conditions;
};

UINTERFACE(MinimalAPI)
class UUAI_Controller : public UInterface
{
	GENERATED_BODY()
};

class CAPSTONEPROJECT_API IUAI_Controller
{
	GENERATED_BODY()

public:
	UAI_Action* GetBestAction() const;
	void SetBestAction(UAI_Action* action, EActionType actionType = EActionType::None);

	virtual void EndAction();
protected:
	void FSM_Tick(const float& DeltaTime);
	void FSM_Reset();

	virtual bool IsAIControlled();

private:
	UAI_Action* bestAction;
	EActionType currentActionType = EActionType::None;
	EDecisionState FSM_State = EDecisionState::Deciding;
	float updateTime = 0.f;

	EActionType DecideBestActionType(TMap<EActionType, FActionSelection>& actionTypes);
	bool DecideBestAction();

	float ScoreAction(const TArray<UAI_Condition*>& conditions);

	bool UpdateTimer(const float& DeltaTime);
	bool AbandonTimer(const float& DeltaTime);
	bool DestinationUpdateTimer(const float& DeltaTime);

	float abandonTime = 0.f;
	float destinationUpdateTime = 0.f;

protected:
	virtual float GetDefaultScore() const;
	virtual float GetUpdateRate() const;
	void SetDestinationUpdateTimer(const float& duration);
	virtual TMap<EActionType, FActionSelection>& GetActions() PURE_VIRTUAL(IUAI_Controller::GetActions, static TMap<EActionType, FActionSelection> emptyMap; return emptyMap;);
	virtual bool DestinationReached() const PURE_VIRTUAL(IUAI_Controller::DestinationReached, return false;);
};
