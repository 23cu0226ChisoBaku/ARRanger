#include "ARRangerAnimInstance.h"

void UARRangerAnimInstance::RequestFinishWalk()
{
    // 歩き状態終了フラグを上げ、終了後の時間を計測
    FinishWalk = true;
    FinishStartTime = GetWorld()->GetTimeSeconds();
}

void UARRangerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 少し時間がたってからIdleへ移行させる
    if (FinishWalk && (GetWorld()->GetTimeSeconds() - FinishStartTime >= FinishDuration))
    {
        FinishWalk = false;
        ShouldMove = false;
    }
}