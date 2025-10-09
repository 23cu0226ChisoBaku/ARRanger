#include "Animations/ARRangerAnimInstance.h"

void UARRangerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
  Super::NativeUpdateAnimation(DeltaSeconds);

  UAnimMontage* curtMontage = GetCurrentActiveMontage();
  if ((curtMontage != nullptr) && !FMath::IsNearlyEqual(Montage_GetPlayRate(curtMontage), AnimModifiedSpeed))
  {
    Montage_SetPlayRate(curtMontage, AnimModifiedSpeed);
  }

  if (IsFalled)
  {
    InFallingTime += DeltaSeconds;
  }
}

void UARRangerAnimInstance::NativePostEvaluateAnimation()
{
  Super::NativePostEvaluateAnimation();

  if (!IsFalled)
  {
    InFallingTime = 0.0f;
  }
}