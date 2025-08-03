// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/Core/ARPhysicsTickObject.h"

FARPhysicsEvaluationResult::FARPhysicsEvaluationResult(const FVector& InPreviousForce)
  : PreviousForce(InPreviousForce)
  , ForceResult{ForceInitToZero}
{ }

UARPhysicsTickObject::UARPhysicsTickObject()
  : PreviousExternalForceResult{ForceInitToZero}
  , EvaluatedExternalForceResult{ForceInitToZero}
  , bIsTerminated{false}
  , bIsCurrentFrameEvaluateFinished{false}
{ }

void UARPhysicsTickObject::BeginTickObject()
{
  if (!bIsTerminated)
  {
    PreviousExternalForceResult = EvaluatedExternalForceResult;
    bIsCurrentFrameEvaluateFinished = false;
    OnBeginTickObject();
  }
}

void UARPhysicsTickObject::Tick(const FARPhysicsTickParameters& TickParams)
{
  if (!bIsTerminated && !bIsCurrentFrameEvaluateFinished)
  {
    FARPhysicsEvaluationResult result{PreviousExternalForceResult};
    OnTick(TickParams, result);
  }
}

void UARPhysicsTickObject::EndTickObject(const FARPhysicsEvaluationResult& EvaluatedResult)
{
  if (!bIsTerminated && !bIsCurrentFrameEvaluateFinished)
  {
    EvaluatedExternalForceResult = EvaluatedResult.ForceResult;
    bIsCurrentFrameEvaluateFinished = true;

    OnEndTickObject(EvaluatedResult);
  }
}

void UARPhysicsTickObject::TerminateTickObject()
{
  bIsTerminated = true;
}

void UARPhysicsTickObject::BeginDestroy()
{
  TerminateTickObject();

  Super::BeginDestroy();
}