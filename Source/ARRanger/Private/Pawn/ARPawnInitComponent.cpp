#include "Pawn/ARPawnInitComponent.h"

#include "Pawn/ARPawnInitData.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"
#include "ActionAbilities/ARGameplayAbilityBase.h"
#include "PlayerComponents/ARChargeAttackComponent.h"

// Sets default values for this component's properties
UARPawnInitComponent::UARPawnInitComponent(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , AbilitySystemComponent{nullptr}
  , PawnInitData{nullptr}
{
  PrimaryComponentTick.bCanEverTick = false;
}

UARPawnInitComponent* UARPawnInitComponent::FindPawnInitComponent(const AActor* InActor)
{
  if (InActor == nullptr)
  {
    return nullptr;
  }

  return ::Cast<UARPawnInitComponent>(InActor->GetComponentByClass(UARPawnInitComponent::StaticClass()));
}

UAbilitySystemComponent* UARPawnInitComponent::GetAbilitySystemComponent() const
{
  return GetARAbilitySystemComponent();
}

void UARPawnInitComponent::OnRegister()
{
  Super::OnRegister();

  const APawn* ownerPawn = GetPawn<APawn>();
  ensureAlwaysMsgf((ownerPawn != nullptr), TEXT("ARPawnInitComponent on [%s] can only be added to Pawn"), *GetNameSafe(GetOwner()));

  TArray<UActorComponent*> pawnInitComponents;
  ownerPawn->GetComponents(UARPawnInitComponent::StaticClass(), pawnInitComponents);
  ensureAlwaysMsgf(pawnInitComponents.Num() == 1, TEXT("Can not add ARPawnInitComponent more than once on [%s]"), *GetNameSafe(GetOwner()));
}

void UARPawnInitComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  UninitializeAbilitySystem();
  UninitializeChargeAttack();

  Super::EndPlay(EndPlayReason);
}

void UARPawnInitComponent::InitializeAbilitySystem(UARAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
  check(InASC != nullptr);
  check(InOwnerActor != nullptr);

  if (AbilitySystemComponent == InASC)
  {
    return;
  }

  // Clean up previous ASC
  if (AbilitySystemComponent != nullptr)
  {
    UninitializeAbilitySystem();
  }

  APawn* ownerPawn = GetPawnChecked<APawn>();
  AActor* existingAvatar = InASC->GetAvatarActor();

  // Clean up previous avatar ASC if it is acting as the ASC's avatar
  if ((existingAvatar != nullptr) && (existingAvatar != ownerPawn))
  {
    if (UARPawnInitComponent* avatarPawnInitComp = FindPawnInitComponent(existingAvatar))
    {
      avatarPawnInitComp->UninitializeAbilitySystem();
    }
  }

  AbilitySystemComponent = InASC;
  AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, ownerPawn);

  if (PawnInitData != nullptr)
  {
    // アビリティ初期化
    int32 inputID = 0;
    const int32 abilityLevel = 1;
    for (TSoftClassPtr<UARGameplayAbilityBase> GA : PawnInitData->Abilities)
    {
      if (GA != nullptr)
      { 
        FGameplayAbilitySpec newAbilitySpec{GA.LoadSynchronous(), abilityLevel, inputID++};
        (void)AbilitySystemComponent->GiveAbility(newAbilitySpec);
      }  
    }
  }
}

void UARPawnInitComponent::InitializeChargeAttack(UARChargeAttackComponent* InCAC)
{
  check(InCAC != nullptr);

  if (ChargeAttackComponent == InCAC)
  {
    return;
  }

  if (ChargeAttackComponent != nullptr)
  {
    UninitializeChargeAttack();
  }

  ChargeAttackComponent = InCAC;

  if (PawnInitData != nullptr)
  {
    for (const FChargeAttackBranchEntry& entry : PawnInitData->ChargeBranchEntries)
    {
      for (const FChargeAttackLeaf& leaf : entry.BranchLeaves)
      {
        ChargeAttackComponent->AddChargeAttack(entry.BranchTag, leaf.TimeThreshold, leaf.ChargeAttackCost, leaf.AttackTag);
      }
    }
  }
} 

void UARPawnInitComponent::UninitializeAbilitySystem()
{
  if (AbilitySystemComponent == nullptr)
  {
    return;
  }

  if (AbilitySystemComponent->GetAvatarActor() != GetOwner())
  {
    return;
  }

  AbilitySystemComponent->CancelAbilities();
  AbilitySystemComponent->ClearAbilityInputStates();
  AbilitySystemComponent->RemoveAllGameplayCues();
  AbilitySystemComponent->ClearActorInfo();
  AbilitySystemComponent = nullptr;
}

void UARPawnInitComponent::UninitializeChargeAttack()
{
  if (ChargeAttackComponent != nullptr)
  {
    ChargeAttackComponent->ClearAllChargeAttacks();
    ChargeAttackComponent = nullptr;
  }
}
