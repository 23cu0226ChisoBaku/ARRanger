#include "GA_Attack.h"

#include "GameFramework/Character.h"

UGA_Attack::UGA_Attack()
{
    // 毎回新しいインスタンスを作る
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Attack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
    {
        UE_LOG(LogTemp, Log, TEXT("Attack ability activated!"));

        // TODO: アニメーション再生や攻撃判定処理をここに書く
    }

    // 即終了する場合
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}