// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Core/ARPhysicsTickObject.h"
#include "Physics/Core/ARPhysicsTickManagerInterface.h"
#include "Internal/ARLoggingHeader.h"

UARPhysicsTickObject::UARPhysicsTickObject()
  : PreviousResult{}
  , EvaluatedResult{}
  , m_internalData{::MakeUnique<FInternalData>()}
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

void UARPhysicsTickObject::SetFrequency(EARPhysicsTickFrequency InFrequency)
{
  PrimaryPhysicsTick.Frequency = InFrequency;
}

void UARPhysicsTickObject::BeginTickObject()
{
  PreviousResult = EvaluatedResult;
  m_internalData->bIsEvaluateFinishedCurrentFrame = false;
  OnBeginTickObject();  
}

void UARPhysicsTickObject::Tick(const FARPhysicsTickParameters& TickParams)
{
  check(m_internalData.IsValid());
  if (!m_internalData->bIsEvaluateFinishedCurrentFrame)
  {
    FARPhysicsEvaluationResult result{};
    OnTick(TickParams, result);

    // TODO For blueprint usage
    // Same as AActor::Tick
    if (GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) || !GetClass()->HasAllClassFlags(CLASS_Native))
    {
      TickOnBlueprint(TickParams.DeltaTime, TickParams.TotalSimTime, result);
    }

    EvaluatedResult = result;
    m_internalData->bIsEvaluateFinishedCurrentFrame = true;
  }
}

void UARPhysicsTickObject::EndTickObject()
{
  check(m_internalData.IsValid());

  if (m_internalData->bIsEvaluateFinishedCurrentFrame)
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
