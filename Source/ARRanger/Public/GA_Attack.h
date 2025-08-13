#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "GA_Attack.generated.h"

UCLASS()
class ARRANGER_API UGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    // コンストラクタでアビリティの基本設定
    UGA_Attack();

    // 実際にアビリティ発動時に呼ばれる関数
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;
};