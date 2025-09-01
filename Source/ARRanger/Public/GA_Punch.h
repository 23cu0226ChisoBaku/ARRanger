#pragma once

#include "Abilities/GameplayAbility.h"
#include "AttackBaseComponent.h"
#include "CoreMinimal.h"

#include "GA_Punch.generated.h"

UCLASS()
class ARRANGER_API UGA_Punch : public UGameplayAbility
{
	GENERATED_BODY()

public:
    //UGA_Punch();

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
    // AttackBaseComponent‚ð•Û‘¶
    UAttackBaseComponent* attackBaseComp = nullptr;

public:
    
};