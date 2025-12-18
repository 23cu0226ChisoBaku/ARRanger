#include "Physics/TickObjects/Magnetic/ARMagneticRepulsionTickObject.h"

#include "Magnetic/IARMagnetizableInterface.h"

#include "Internal/ARLoggingHeader.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ARMagneticRepulsionTickObject)

namespace
{
  /**斥力上方向特殊判定閾値 */
  constexpr double TOP_DETECTED_COMPONENT_NORM_Z_THRESHOLD = 0.7f;
}

// TODO Should remove all magic number
void UARMagneticRepulsionTickObject::OnTick(const FARPhysicsTickParameters& TickParams, FARPhysicsEvaluationResult& Result)
{
  if (Target == nullptr)
  {
    AR_LOG(LogARPhysics, Error, TEXT("Target is nullptr. Magnetic repulsion task denied"));
    return;
  }

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

    AActor* magnetizedObjectActor = magnetizedObject->GetActor();
    const FVector impactDir = (targetActor->GetActorLocation() - magnetizedObjectActor->GetActorLocation()).GetUnsafeNormal();

    // ターゲットが磁力オブジェクトの上にいる時斥力が発生する場合
    // ターゲットの移動速度の方向へ押し出す
    if (impactDir.Z > TOP_DETECTED_COMPONENT_NORM_Z_THRESHOLD)
    {
      // TODO 移動速度方向へ押し出す力の大きさ
      constexpr double velocityPushPower = 1500.0;
      const FVector pushUpForce{0.0, 0.0, 1200.0};

      const FVector targetVeloNorm = targetActor->GetVelocity().GetSafeNormal();
      const FVector launchVelo = targetVeloNorm * velocityPushPower + pushUpForce;
      Result.ForceResult += launchVelo;
    }
    // 上じゃない場合は主に水平方向へ押し出し
    else
    {
      // TODO 水平へ押し出す力の大きさ
      constexpr double pushPower = 800.0;
      const FVector pushUpForce{0.0, 0.0, 200.0};

      const FVector horizontalDir = FVector{impactDir.X, impactDir.Y, 0.0}.GetSafeNormal();
      const FVector knockBackVelo = horizontalDir * pushPower + pushUpForce;
      Result.ForceResult += knockBackVelo;
    }
  }

}

void UARMagneticRepulsionTickObject::OnPostTickObject()
{
  // 斥力結果をターゲットに送る
  if (Target != nullptr)
  {
    FARMagneticForceResult result{};
    result.FinalForce = GetEvaluatedResult().ForceResult;
    Target->OnRepulsionEvaluated(result);
  }


  Super::OnPostTickObject();
}