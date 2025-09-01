#include "GA_Punch.h"

#include "ARRangerCharacter.h"

void UGA_Punch::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetOwningActorFromActorInfo()))
    {
        Char->GA_PunchInstance = this;
    }
}


void UGA_Punch::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    /*bIsAttacked = false;
    bIsStrongAttack = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;*/

    AARRangerCharacter* Char = Cast<AARRangerCharacter>(GetAvatarActorFromActorInfo());
    Char->SetIsAttacked(false);
    Char->SetIsStrongAttacked(false);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}