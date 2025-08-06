// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/TickObjects/Magnetic/ARMagneticAttractionTickObject.h"

#include "IARMagnetizableInterface.h"
#include "Physics/Core/ARPhysicsEngineProxy.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARMagneticAttractionTickObject)

namespace
{
  // FIXME 一時的な定数
  constexpr float CONST_PROP = 25.0f;
  constexpr float MAGNETIC_VALUE = 20.0f;
}

void UARMagneticAttractionTickObject::OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result)
{
  if ((Target == nullptr) || (PhysicsEngineProxy == nullptr))
  {
    AR_LOG(LogARPhysics, Error, TEXT("TargetObject or EngineProxy is invalid"));
    return;
  }

  // 斥力計算
  AActor* targetActor = Target->GetActor();
  if (targetActor == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Target actor is nullptr.Do you override IARMagnetizableInterface::GetActor()? "));
    return;
  }

  for (const auto& magnetizedObject : AffectedMagnetizedObjects)
  {
    if ((magnetizedObject == nullptr) || (magnetizedObject->GetActor() == nullptr))
    {
      continue;
    }

    // TODO Maybe should merge this things into EngineProxy?
    {
      const AActor* magnetizedActor = magnetizedObject->GetActor();
      const FVector directionTo = magnetizedActor->GetActorLocation() - targetActor->GetActorLocation();
      const FVector pushForce = directionTo.GetUnsafeNormal() * CONST_PROP * FMath::Pow(MAGNETIC_VALUE, 2.0f) / directionTo.SizeSquared();
      
      Result.ForceResult += pushForce;

      // TODO testCode
      targetActor->AddActorWorldOffset(pushForce, true);

      AR_LOG(LogARPhysics, Warning, TEXT("Push force: X:[%f], Y:[%f], Z:[%f]"), pushForce.X, pushForce.Y, pushForce.Z);
    }

    FARPhysicsSimulationParam params{*Target, *magnetizedObject};
    PhysicsEngineProxy->SimulateAttraction(params);
  }
  


}