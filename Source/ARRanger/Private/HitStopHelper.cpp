//*************************************************
// ヒットストップの処理を行うヘルパークラス
//*************************************************

#include "HitStopHelper.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

/**
 * @brief 指定した期間、指定した速度でモーションさせる
 * 
 * @param 呼び出すオブジェクト、止める期間、モーション速度
 */
FTimerHandle UHitStopHelper::StartHitStop(UObject* object, float duration, float motionSpeed)
{
    UWorld* world = GEngine->GetWorldFromContextObjectChecked(object);
    if (world == nullptr) 
    {
        UE_LOG(LogTemp, Warning, TEXT("The world argument is null"));
        return FTimerHandle{};
    }

    if(duration <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("The hit stop duration is 0 or less"));
        return FTimerHandle{};
    }

    /*指定した時間でモーションさせる*/
    UGameplayStatics::SetGlobalTimeDilation(world, motionSpeed);

    FTimerHandle TimerHandle{};
    TimerHandle.Invalidate();

    /*指定時間後にモーションを戻す*/
    world->GetTimerManager().SetTimer(TimerHandle, [world]()
    {
        if (world != nullptr)
        {
            UGameplayStatics::SetGlobalTimeDilation(world, 1.f);
        }
    }, duration * motionSpeed, false);

    return TimerHandle;
}