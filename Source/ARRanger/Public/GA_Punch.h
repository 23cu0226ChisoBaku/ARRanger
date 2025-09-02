#pragma once

#include "Abilities/GameplayAbility.h"
#include "AttackData.h"
#include "CoreMinimal.h"

#include "GA_Punch.generated.h"

class UAttackBaseComponent;

UCLASS()
class ARRANGER_API UGA_Punch : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UGA_Punch();

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
    UAttackBaseComponent* attackBaseComp;

    // 最大コンボ回数
    int32 MaxCombo = 3;

    // コンボ時アニメーションモンタージュのセクション名を取得
    FName GetPunchSectionName(int32 Index) const;

protected:
    // パンチのデータ
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData PunchData;

public:
    // コンボ用のモンタージュ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
    TArray<UAnimMontage*> ComboMontages;

    // パンチの際に呼び出される
    void StartPunch();

    // コンボスタート
    UFUNCTION(BlueprintCallable)
    void ComboWindowStart();

    // コンボ終了
    UFUNCTION(BlueprintCallable)
    void ComboWindowEnd();

    // パンチのAnimNotifyの通知を受け取る
    UFUNCTION(BlueprintCallable)
    void PunchHitNotify();
};