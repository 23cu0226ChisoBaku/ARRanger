// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Core/ARPhysicsTickObject.h"
#include "Physics/Core/ARPhysicsTickManagerInterface.h"
#include "Internal/ARLoggingHeader.h"

UARPhysicsTickObject::UARPhysicsTickObject()
  : PreviousResult{}
  , EvaluatedResult{}
{ }

void UARPhysicsTickObject::RegisterPhysicsTickFunction()
{
  check(!PrimaryPhysicsTick.IsTickFunctionRegistered());

  PrimaryPhysicsTick.TargetObject = this;
  PrimaryPhysicsTick.SetEnable(true);
  PrimaryPhysicsTick.RegisterPhysicsTickFunction();
}

void UARPhysicsTickObject::TickPhysics(const FARPhysicsTickParameters& TickParams)
{
  BeginTickObject();

  Tick(TickParams);

  EndTickObject();
}

void UARPhysicsTickObject::UnregisterPhysicsTickFunction()
{
  PrimaryPhysicsTick.UnregisterPhysicsTickFunction();
}

void UARPhysicsTickObject::SetFrequency(EPhysicsExecuteFrequency InFrequency)
{
  PrimaryPhysicsTick.Frequency = InFrequency;
}

void UARPhysicsTickObject::BeginTickObject()
{
  PreviousResult = EvaluatedResult;
  bIsEvaluateFinishedCurrentFrame = false;
  OnBeginTickObject();  
}

void UARPhysicsTickObject::Tick(const FARPhysicsTickParameters& TickParams)
{
  if (!bIsEvaluateFinishedCurrentFrame)
  {
    FARPhysicsEvaluationResult result{};
    OnTick(TickParams, result);

    // For blueprint usage
    // Same as AActor::Tick
    if (GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) || !GetClass()->HasAllClassFlags(CLASS_Native))
    {
      TickOnBlueprint(TickParams.DeltaTime, result);
    }

    EvaluatedResult = result;
    bIsEvaluateFinishedCurrentFrame = true;
  }
}

void UARPhysicsTickObject::EndTickObject()
{
  if (bIsEvaluateFinishedCurrentFrame)
  {
    OnEndTickObject();
  }
}



void UARPhysicsTickObject::BeginDestroy()
{
  UnregisterPhysicsTickFunction();

  Super::BeginDestroy();
}

void FARPhysicsTickFunction::ExecuteTick(const FARPhysicsTickParameters& TickParams)
{
  if (IsValid(TargetObject))
  {
    TargetObject->TickPhysics(TickParams);
  }
}
