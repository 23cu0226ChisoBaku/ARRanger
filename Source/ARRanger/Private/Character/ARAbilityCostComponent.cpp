// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ARAbilityCostComponent.h"

float FAbilityCostEntry::GetResourceNum() const
{
  return ResourceNum;
}

float FAbilityCostEntry::GetMaxResourceNum() const
{
  return MaxResourceNum;
}

void FAbilityCostEntry::SetResourceNum(float NewResourceNum, bool& bOutResult)
{
  bOutResult = false;
  const float prevResource = GetResourceNum();
  ResourceNum = FMath::Clamp(prevResource + NewResourceNum, 0.0f, GetMaxResourceNum());
  
  if (OnAbilityCostChanged.IsBound())
  {
    bOutResult = !FMath::IsNearlyEqual(prevResource, GetResourceNum());
    OnAbilityCostChanged.Broadcast(prevResource, GetResourceNum(), bOutResult);
  }
}

// Sets default values for this component's properties
UARAbilityCostComponent::UARAbilityCostComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UARAbilityCostComponent::HandleAbilityCostChanged(const FGameplayTag& InAbilityCostTag, float InResourceChangeNum, bool& bOutResult)
{
  FAbilityCostEntry* foundEntry = FindEntryByTag(InAbilityCostTag);
  if (foundEntry != nullptr)
  {
    const float prevResourceNum = foundEntry->GetResourceNum();
    foundEntry->SetResourceNum(InResourceChangeNum, bOutResult);
    const float curtResourceNum = foundEntry->GetResourceNum();
    OnAbilityCostApplied.Broadcast(this, InAbilityCostTag, prevResourceNum, curtResourceNum, bOutResult);
  }
}

FDelegateHandle UARAbilityCostComponent::AddDelegateToAbilityCost(const FGameplayTag& InAbilityCostTag, AbilityCostDelegate_Native&& InDelegate)
{

  FAbilityCostEntry* foundEntry = FindEntryByTag(InAbilityCostTag);
  if (foundEntry != nullptr)
  {
    return foundEntry->OnAbilityCostChanged.Add(::MoveTemp(InDelegate));
  }

  // Return invalid handle if we can not find Ability cost
  return FDelegateHandle{};
}

void UARAbilityCostComponent::RemoveDelegateInAbilityCost(const FGameplayTag& InAbilityCostTag, FDelegateHandle InHandle)
{
  FAbilityCostEntry* foundEntry = FindEntryByTag(InAbilityCostTag);
  if (foundEntry != nullptr)
  {
    (void)foundEntry->OnAbilityCostChanged.Remove(InHandle);
  }
}

FAbilityCostEntry* UARAbilityCostComponent::FindEntryByTag(const FGameplayTag& InAbilityCostTag)
{
  const auto searchEntryHandler = [&InAbilityCostTag](const FAbilityCostEntry& Element)
  {
    return Element.AbilityCostTag == InAbilityCostTag;
  };

  return AbilityCostEntries.FindByPredicate(searchEntryHandler);
}
