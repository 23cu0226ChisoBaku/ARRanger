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
  check(!m_internalData->bIsTerminated);
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

void UARPhysicsTickObject::BeginTickObject()
{
  if (!m_internalData->bIsTerminated)
  {
    PreviousResult = EvaluatedResult;
    m_internalData->bIsEvaluateFinishedCurrentFrame = false;
    OnBeginTickObject();
  }
}

void UARPhysicsTickObject::Tick(const FARPhysicsTickParameters& TickParams)
{
  check(m_internalData.IsValid());
  if (!m_internalData->bIsTerminated && !m_internalData->bIsEvaluateFinishedCurrentFrame)
  {
    FARPhysicsEvaluationResult result{};
    OnTick(TickParams, result);

    // TODO For blueprint usage
    // Same as AActor::Tick
    if (GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) || !GetClass()->HasAllClassFlags(CLASS_Native))
    {
      TickOnBlueprint(TickParams, result);
    }

    EvaluatedResult = result;
    m_internalData->bIsEvaluateFinishedCurrentFrame = true;
  }
}

void UARPhysicsTickObject::EndTickObject()
{
  check(m_internalData.IsValid());

  if (!m_internalData->bIsTerminated && !m_internalData->bIsEvaluateFinishedCurrentFrame)
  {
    OnEndTickObject();
  }
}

void UARPhysicsTickObject::TerminateTickObject()
{
  if (m_internalData.IsValid())
  {
    m_internalData->bIsTerminated = true;
  }

  PrimaryPhysicsTick.UnregisterPhysicsTickFunction();
}

void UARPhysicsTickObject::BeginDestroy()
{
  TerminateTickObject();

  Super::BeginDestroy();
}

void FARPhysicsTickFunction::ExecuteTick(const FARPhysicsTickParameters& TickParams)
{
  if (IsValid(TargetObject) && !TargetObject->IsTerminated())
  {
    TargetObject->TickPhysics(TickParams);

    AR_LOG(LogARPhysics, Log, TEXT("Run FARPhysicsTickFunction ExecuteTick"));
  }
}
