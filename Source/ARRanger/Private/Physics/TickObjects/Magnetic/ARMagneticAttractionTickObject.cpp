// Fill out your copyright notice in the Description page of Project Settings.

#include "Physics/TickObjects/Magnetic/ARMagneticAttractionTickObject.h"

#include "IARMagnetizableInterface.h"
#include "Physics/Core/ARPhysicsEngineProxy.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARMagneticAttractionTickObject)

namespace
{
  // 比例定数ｋ（単位：N・m^2/Wb^2）約6.33 * 10^4
  // URL: https://hegtel.com/ac-coulomb-magnet.html
  // URL: https://hegtel.com/jikai-tsuyosa.html
  // URL: https://rikeilabo.com/magnetic-field-and-magnetic-flux-density
  constexpr float PROPORTIONALITY_CONSTANT = 6.33e4f;
  constexpr float MAGNETIC_VALUE = 60.0f;
}

UARMagneticAttractionTickObject::UARMagneticAttractionTickObject()
{
}

void UARMagneticAttractionTickObject::OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result)
{
  const AActor* targetActor = Target->GetActor();
  if (targetActor == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Target actor is nullptr.Do you override IARMagnetizableInterface::GetActor()? "));
    return;
  }
  
  // 斥力計算
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
      const FVector pushForce = directionTo.GetUnsafeNormal() * PROPORTIONALITY_CONSTANT * MAGNETIC_VALUE * MAGNETIC_VALUE / directionTo.SizeSquared() * 0.0001f;
      
      Result.ForceResult += pushForce;

    }
  }
}

void UARMagneticAttractionTickObject::OnEndTickObject()
{
  if (Target == nullptr)
  {
    return;
  }

  FARMagneticForceResult result;
  result.FinalForce = GetEvaluatedResult().ForceResult;

  Target->OnAttractionEvaluated(result);

  Super::OnEndTickObject();
}