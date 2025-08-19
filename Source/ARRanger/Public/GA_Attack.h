#pragma once

#include "Abilities/GameplayAbility.h"
#include "AttackData.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "GA_Attack.generated.h"

UCLASS()
class ARRANGER_API UGA_Attack : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UGA_Attack();

    // Ability開始時
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    // Ability終了時
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    // ===== AttackComponentから移植した変数 =====
    bool bIsAttacked = false;
    bool bIsStrongAttack = false;
    bool bIsBlowedAwayEnemy = false;
    bool bIsAttractingEnemy = false;

    // 現在のコンボ段階
    int32 ComboCount = 0;

    // 最大コンボ回数
    int32 MaxCombo = 3;

    // パンチ・キックのデータ
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData PunchData;

    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData KickData;

    UPROPERTY(EditAnywhere, Category = "Effects")
    TSubclassOf<AActor> HitEffectActor;

    // ===== 移植した関数 =====
    void RotateOwnerToTarget();
    void PlayAttackMontage(const FAttackData& Attack);
    void AttackHit(const FAttackData& Attack);

    // Montage終了イベント
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
    // コンボ用のモンタージュ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    TArray<UAnimMontage*> ComboMontages;

    // パンチの際に呼び出される
    void StartPunch();

    // キックの際に呼び出される
    void StartKick();

    // パンチのAnimNotifyの通知を受け取る
    UFUNCTION(BlueprintCallable)
    void PunchHitNotify();

    // キックのAnimNotifyの通知を受け取る
    UFUNCTION(BlueprintCallable)
    void KickHitNotify();

    // 引き寄せ中フラグを取得
    bool GetIsAttractingEnemy() { return bIsAttractingEnemy; }

    // 攻撃中フラグを取得
    bool GetIsAttacked() { return bIsAttacked; }

    //強攻撃フラグを取得
    bool GetIsStrongAttacked() { return bIsStrongAttack; }
};