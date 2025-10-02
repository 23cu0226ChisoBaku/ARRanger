//*************************************************
// ヒットストップの処理を行うアニメーション通知ステート
//*************************************************

#include "Animations/AnimNotifies/AnimNotifyState_HitStop.h"

void UAnimNotifyState_HitStop::NotifyBegin(USkeletalMeshComponent * meshComp, UAnimSequenceBase * animation, float totalDuration, const FAnimNotifyEventReference& eventReference)
{
  Super::NotifyBegin(meshComp, animation, totalDuration, eventReference);

    if (meshComp != nullptr && meshComp->GetWorld() != nullptr)
    {
        UWorld* world = meshComp->GetWorld();

        /*元のモーション速度を保持*/
        m_OriginalMotionSpeed = world->GetWorldSettings()->GetEffectiveTimeDilation();

        /*指定のモーション速度に変更*/
        UGameplayStatics::SetGlobalTimeDilation(world, m_HitStopMotionSpeed);
    }
}

void UAnimNotifyState_HitStop::NotifyEnd(USkeletalMeshComponent * meshComp, UAnimSequenceBase * animation, const FAnimNotifyEventReference& eventReference)
{
  Super::NotifyEnd(meshComp, animation, eventReference);

    if (meshComp != nullptr && meshComp->GetWorld() != nullptr)
    {
        /*元のモーション速度に戻す*/
        meshComp->GetWorld()->GetWorldSettings()->SetTimeDilation(m_OriginalMotionSpeed);
    }
}

FString UAnimNotifyState_HitStop::GetNotifyName_Implementation() const
{
    return FString::Printf(TEXT("Modified Motion Speed : [%f]"), m_HitStopMotionSpeed);
}


