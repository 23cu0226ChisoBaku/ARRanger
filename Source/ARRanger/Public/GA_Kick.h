#pragma once

#include "Abilities/GameplayAbility.h"
#include "AttackData.h"
#include "CoreMinimal.h"

#include "GA_Kick.generated.h"

class UAttackBaseComponent;

UCLASS()
class ARRANGER_API UGA_Kick : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Kick();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility, bool bWasCancelled
    ) override;

private:
    // AttackBaseComponentを保存
    UAttackBaseComponent* attackBaseComp = nullptr;

    // チャージ開始時間
    float chargeStartTime = 0.0f;

    // チャージ継続時間
    float chargeDuration = 0.0f;

    // チャージの段階
    int32 chargeLevel = 0;

protected:
    // キックのデータ
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData KickData;

    // Montage終了イベント
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
    // キックの際に呼び出される
    void StartKick();

    // キックのAnimNotifyの通知を受け取る
    UFUNCTION(BlueprintCallable)
    void KickHitNotify();

    // ボタンを離した際に呼び出される
    void InputReleased();
};