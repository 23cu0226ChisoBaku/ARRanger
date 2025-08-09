// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/TickObjects/Magnetic/ARMagneticRepulsionTickObject.h"
#include "IARMagnetizableInterface.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARMagneticRepulsionTickObject)

namespace
{
  constexpr float TOP_DETECTED_COMPONENT_NORM_Z_THRESHOLD = 0.7f;
}

UARMagneticRepulsionTickObject::UARMagneticRepulsionTickObject()
{
  PrimaryPhysicsTick.Frequency = EARPhysicsTickFrequency::TF_Once;
}

void UARMagneticRepulsionTickObject::OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result)
{
  check(Target != nullptr);
  AActor* targetActor = Target->GetActor();
  if (targetActor == nullptr)
  {
    return;
  }


  bool zOverride = true;
  for (const auto& magnetizedObject : AffectedMagnetizedObjects)
  {
    if ((magnetizedObject == nullptr) || (magnetizedObject->GetActor() == nullptr))
    {
      continue;
    }

    AActor* magnetizedObjectActor = magnetizedObject->GetActor();
    const FVector impactDir = (targetActor->GetActorLocation() - magnetizedObjectActor->GetActorLocation()).GetUnsafeNormal();

    if (impactDir.Z > TOP_DETECTED_COMPONENT_NORM_Z_THRESHOLD)
    {
      const FVector targetVelo = targetActor->GetVelocity();
      if (targetVelo.SizeSquared() > DOUBLE_KINDA_SMALL_NUMBER)
      {
        const FVector forwardDir = targetVelo.GetUnsafeNormal();
        const FVector launchVelo = forwardDir * 1200.0 + FVector{0.0, 0.0, 800.0};
        Result.ForceResult += launchVelo;
      }
      else
      {
        const FVector launchVelo = FVector{0.0, 0.0, 1400.0f};
        Result.ForceResult += launchVelo;
      }
    }
    else
    {
      zOverride = false;
      const FVector horizontalDir = FVector{impactDir.X, impactDir.Y, 0.0}.GetUnsafeNormal();
      const FVector knockBackVelo = horizontalDir * 800.0f + FVector{0.0, 0.0, 200.0};
      Result.ForceResult += knockBackVelo;
    }

    // TODO: Test Code
    if (ACharacter* targetCharacter = Cast<ACharacter>(targetActor))
    {
      targetCharacter->LaunchCharacter(Result.ForceResult, true, zOverride);
    }
    else
    {
      if (UPrimitiveComponent* primitiveComp = targetActor->GetComponentByClass<UPrimitiveComponent>())
      {
        primitiveComp->AddImpulse(Result.ForceResult);
      }
    }
  }

  Target->OnMagneticForceEvaluated(Result.ForceResult);
}