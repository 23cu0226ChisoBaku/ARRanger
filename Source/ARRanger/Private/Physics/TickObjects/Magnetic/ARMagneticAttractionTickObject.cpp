#include "Physics/TickObjects/Magnetic/ARMagneticAttractionTickObject.h"

#include "Magnetic/IARMagnetizableInterface.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARMagneticAttractionTickObject)

namespace
{
  // 比例定数ｋ（単位：N・m^2/Wb^2）約6.33 * 10^4
  // URL: https://hegtel.com/ac-coulomb-magnet.html
  // URL: https://hegtel.com/jikai-tsuyosa.html
  // URL: https://rikeilabo.com/magnetic-field-and-magnetic-flux-density
  constexpr double PROPORTIONALITY_CONSTANT = 6.33e4;
  constexpr double MAGNETIC_VALUE = 60.0;
  constexpr double ADJUSTMENT_COEFFICIENT = 0.0001;
}

void UARMagneticAttractionTickObject::OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result)
{
  if (Target == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Target is nullptr. Magnetic attraction task denied"));
    return;
  }

  const AActor* targetActor = Target->GetActor();
  if (targetActor == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Target actor is nullptr.Do you override IARMagnetizableInterface::GetActor()? "));
    return;
  }
  
  // 引力計算
  for (const auto& magnetizedObject : AffectedMagnetizedObjects)
  {
    if ((magnetizedObject == nullptr) || (magnetizedObject->GetActor() == nullptr))
    {
      continue;
    }

    // クーロンの法則に基づいて引力を計算する
    // F = (k * m1 * m2) / (r^2)
    {
      const AActor* magnetizedActor = magnetizedObject->GetActor();
      const FVector directionTo = magnetizedActor->GetActorLocation() - targetActor->GetActorLocation();
      const FVector pushForce = directionTo.GetSafeNormal() 
                                * /**比例定数ｋ */PROPORTIONALITY_CONSTANT 
                                * /**m1 */MAGNETIC_VALUE * /**m2 */MAGNETIC_VALUE 
                                / /**(r^2) */directionTo.SizeSquared() 
                                * /**調整係数 */ADJUSTMENT_COEFFICIENT;
      
      // 引力を重ねる
      Result.ForceResult += pushForce;
    }
  }
}

void UARMagneticAttractionTickObject::OnPostTickObject()
{
  // 引力結果をターゲットに送る
  if (Target != nullptr)
  {
    FARMagneticForceResult result{};
    result.FinalForce = GetEvaluatedResult().ForceResult;
    Target->OnAttractionEvaluated(result);
  }

  Super::OnPostTickObject();
}