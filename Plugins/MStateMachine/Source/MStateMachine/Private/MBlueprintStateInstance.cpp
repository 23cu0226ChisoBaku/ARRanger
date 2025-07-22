// Fill out your copyright notice in the Description page of Project Settings.


#include "MBlueprintStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MBlueprintStateInstance)

UWorld* UMBlueprintStateInstance::GetWorld() const
{
  /**
   * ブループリントを操作している時OuterはUPackage
   */
  const UPackage* package = ::Cast<UPackage>(GetOuter());
  if (package != nullptr)
  {
    return GWorld;
  }

  return Super::GetWorld();
}

void UMBlueprintStateInstance::OnEnterState(const FStateTransitionParameters& TransParams)
{
  K2_BlueprintEnterState(TransParams);
}

void UMBlueprintStateInstance::OnTickState(const FStateTickParameters& TickParams)
{
  K2_BlueprintTickState(TickParams);
}

void UMBlueprintStateInstance::OnExitState(const FStateTransitionParameters& TransParams)
{
  K2_BlueprintExitState(TransParams);
}