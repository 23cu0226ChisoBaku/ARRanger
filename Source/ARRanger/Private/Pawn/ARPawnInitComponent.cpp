// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/ARPawnInitComponent.h"

#include "Pawn/ARPawnInitData.h"
#include "ActionAbilities/ARAbilitySystemComponent.h"
#include "ActionAbilities/ARGameplayAbilityBase.h"
#include "ActionAbilities/Attributes/ARAttributeSet.h"

#include "PlayerComponents/ARChargeAttackComponent.h"

// Sets default values for this component's properties
UARPawnInitComponent::UARPawnInitComponent(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
  , AbilitySystemComponent{nullptr}
  , PawnInitData{nullptr}
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;

  // ...
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

// Called when the game starts
void UARPawnInitComponent::BeginPlay()
{
  Super::BeginPlay();
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
    // TODO Make inputID and abilityLevel hard coding temporary
    int32 inputID = 0;
    const int32 abilityLevel = 1;
    for (TSoftClassPtr<UARGameplayAbilityBase> GA : PawnInitData->Abilities)
    {
      // Initialize Abilities
      if (GA != nullptr)
      { 
        FGameplayAbilitySpec newAbilitySpec{GA.LoadSynchronous(), abilityLevel, inputID++};
        FGameplayAbilitySpecHandle newAbilitySpecHandle = AbilitySystemComponent->GiveAbility(newAbilitySpec);
      }  
    }

    for (TSoftClassPtr<UARAttributeSet> AS : PawnInitData->AttributeSets)
    {
      // Add attribute set
      if (AS != nullptr)
      {
        TSubclassOf<UARAttributeSet> ASType = AS.LoadSynchronous();
        if (ASType != nullptr)
        {
          UARAttributeSet* newSet = NewObject<UARAttributeSet>(AbilitySystemComponent->GetOwner(), ASType);
          AbilitySystemComponent->AddAttributeSetSubobject(newSet);
        }
      }
    }
  }
}

void UARPawnInitComponent::InitializeChargeAttack(UARChargeAttackComponent* InCAC)
{
  check(InCAC != nullptr);

  // TODO Use Pawn Init Data to add charge branch
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
