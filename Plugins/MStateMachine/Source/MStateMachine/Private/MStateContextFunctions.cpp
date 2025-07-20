// Fill out your copyright notice in the Description page of Project Settings.

#include "MStateContextFunctions.h"

#include "MStateContext.h"
#include "MStateInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MStateContextFunctions)

namespace
{
  bool IsContextValid(const UObject* WorldContextObject, TSharedPtr<FMStateContext, ESPMode::NotThreadSafe> Context);
}

UObject* UMStateContextFunctions::GetOwnerByTransitionParams(const UObject* WorldContextObject, const FStateTransitionParameters& InParameters)
{
  if (!IsContextValid(WorldContextObject, InParameters.Context))
  {
    return nullptr;
  }

  return InParameters.Context->GetOwner();
}

AController* UMStateContextFunctions::GetOwnerControllerByTransitionParams(const UObject* WorldContextObject, const FStateTransitionParameters& InParameters)
{
  if (!IsContextValid(WorldContextObject, InParameters.Context))
  {
    return nullptr;
  }

  return InParameters.Context->GetOwnerController();
}

UObject* UMStateContextFunctions::GetOwnerByTickParams(const UObject* WorldContextObject, const FStateTickParameters& InParameters)
{
  if (!IsContextValid(WorldContextObject, InParameters.Context))
  {
    return nullptr;
  }

  return InParameters.Context->GetOwner();
}

AController* UMStateContextFunctions::GetOwnerControllerByTickParams(const UObject* WorldContextObject, const FStateTickParameters& InParameters)
{
  if (!IsContextValid(WorldContextObject, InParameters.Context))
  {
    return nullptr;
  }

  return InParameters.Context->GetOwnerController();
}

namespace
{
  bool IsContextValid(const UObject* WorldContextObject, TSharedPtr<FMStateContext, ESPMode::NotThreadSafe> Context)
  {
    if ((WorldContextObject == nullptr) || !Context.IsValid())
    {
      return false;
    }
  
    const UWorld* world = WorldContextObject->GetWorld();
    if (world != Context->GetWorld())
    {
      return false;
    }
  
    return true;
  }
}