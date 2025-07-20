// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "MStateMachineLogChannels.h"

#include "MStateInstance.generated.h"

class UMStateMachineComponent;

UENUM(BlueprintType)
enum class EStateTransitionType : uint8
{
  Enter,
  Exit,
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FStateTransitionParameters
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly)
  FGameplayTag TransitionTag = FGameplayTag::EmptyTag;

  UPROPERTY(BlueprintReadOnly)
  EStateTransitionType Transition = EStateTransitionType::Enter;

  TSharedPtr<class FMStateContext, ESPMode::NotThreadSafe> Context;
};

USTRUCT(BlueprintType)
struct FStateTickParameters
{
  GENERATED_BODY()

  UPROPERTY(BlueprintReadOnly)
  float DeltaTime = 0.0f;

  TSharedPtr<class FMStateContext, ESPMode::NotThreadSafe> Context;
};

struct FStateInitializationParameters
{
  TObjectPtr<AActor> OwnerActor;

  TObjectPtr<UMStateMachineComponent> OwnerStateMachineComponent;
};

struct FStateUninitializationParameters
{

};




UCLASS(Abstract, BlueprintType)
class UMStateInstance : public UObject
{
	GENERATED_BODY()

public:
	UMStateInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  MSTATEMACHINE_API void InitializeState(const FStateInitializationParameters& InitParams);
  MSTATEMACHINE_API void UninitializeState(const FStateUninitializationParameters& UninitParams);

public:
	MSTATEMACHINE_API void EnterState(const FStateTransitionParameters& TransParams);
	MSTATEMACHINE_API void TickState(const FStateTickParameters& TickParams);
	MSTATEMACHINE_API void ExitState(const FStateTransitionParameters& TransParams);
  
protected:
  MSTATEMACHINE_API virtual void OnEnterState(const FStateTransitionParameters& TransParams) { };
  MSTATEMACHINE_API virtual void OnTickState(const FStateTickParameters& TickParams) { };
  MSTATEMACHINE_API virtual void OnExitState(const FStateTransitionParameters& TransParams) { };
  MSTATEMACHINE_API virtual void OnInitializeState(const FStateInitializationParameters& InitParams) { };
  MSTATEMACHINE_API virtual void OnUninitializeState(const FStateUninitializationParameters& UninitParams) { };
	
};
