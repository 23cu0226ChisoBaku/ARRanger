// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerComponents/ARChargeAttackComponent.h"

#include "GameplayTagContainer.h"

// Sets default values for this component's properties
UARChargeAttackComponent::UARChargeAttackComponent()
  : m_chargeAttackTrees{}
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UARChargeAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UARChargeAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  ClearAllChargeAttacks();
}


// Called every frame
void UARChargeAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UARChargeAttackComponent::AddChargeAttack(const FGameplayTag& InBranchTag, float InChargeTimeThreshold, float InChargeAttackCost, const FGameplayTag& InChargeAttackTag)
{
  if (!m_chargeAttackTrees.Contains(InBranchTag))
  {
    m_chargeAttackTrees.Emplace(InBranchTag, {});
  }

  const int32 prevNum = m_chargeAttackTrees[InBranchTag].Num();
  m_chargeAttackTrees[InBranchTag].AddUnique({InChargeTimeThreshold, InChargeAttackCost, InChargeAttackTag});

  // Sort as descending order
  if (prevNum != m_chargeAttackTrees[InBranchTag].Num())
  {
    static auto sortPred = [](const FChargeAttackInfo& A, const FChargeAttackInfo& B)
    { 
      return A.TimeThreshold > B.TimeThreshold;
    };
    m_chargeAttackTrees[InBranchTag].Sort(sortPred);
  }
}

FGameplayTag UARChargeAttackComponent::EvaluateCharge(float InTimeCharged, const FGameplayTag& InBranchTag) const
{
  FGameplayTag evaluatedTag = FGameplayTag::EmptyTag;
  
  // TODO
  if (m_chargeAttackTrees.Contains(InBranchTag))
  {
    for (const FChargeAttackInfo& leaf : m_chargeAttackTrees[InBranchTag])
    {
      if (leaf.TimeThreshold < InTimeCharged)
      {
        evaluatedTag = leaf.ChargeAttackTag;
        break;
      }
    }
  }

  return evaluatedTag;
}

void UARChargeAttackComponent::ClearAllChargeAttacks()
{
  m_chargeAttackTrees.Reset();
}

float UARChargeAttackComponent::GetChargeTimeThresholdByAttackTag(const FGameplayTag& InChargeAttackTag) const
{
  for (const auto& [ _ , branches ] : m_chargeAttackTrees)
  {
    for (const FChargeAttackInfo& leaf : branches)
    {
      if (leaf.ChargeAttackTag.MatchesTagExact(InChargeAttackTag))
      {
        return leaf.TimeThreshold;
      }
    }
  }

  return 0.0;
}

float UARChargeAttackComponent::GetChargeAttackCostByAttackTag(const FGameplayTag& InChargeAttackTag) const
{
  for (const auto& [ _ , branches ] : m_chargeAttackTrees)
  {
    for (const FChargeAttackInfo& leaf : branches)
    {
      if (leaf.ChargeAttackTag.MatchesTagExact(InChargeAttackTag))
      {
        return leaf.AttackCost;
      }
    }
  }

  return 0.0;
}


float UARChargeAttackComponent::GetNextChargeTimeThresholdByAttackTag(const FGameplayTag& InChargeAttackTag) const
{
  for (const auto& [ _ , branches ] : m_chargeAttackTrees)
  {
    for (int32 index = 0; index < branches.Num(); ++index)
    {
      if (branches[index].ChargeAttackTag.MatchesTagExact(InChargeAttackTag))
      {
        return (index > 0) ? branches[index - 1].TimeThreshold : branches[index].TimeThreshold;
      }
    } 
  }

  return 0.0f;
}

float UARChargeAttackComponent::GetNextChargeAttackCostByAttackTag(const FGameplayTag& InChargeAttackTag) const
{
    for (const auto& [ _ , branches ] : m_chargeAttackTrees)
  {
    for (int32 index = 0; index < branches.Num(); ++index)
    {
      if (branches[index].ChargeAttackTag.MatchesTagExact(InChargeAttackTag))
      {
        return (index > 0) ? branches[index - 1].AttackCost : branches[index].AttackCost;
      }
    } 
  }

  return 0.0f;
}

bool UARChargeAttackComponent::FChargeAttackInfo::operator==(const UARChargeAttackComponent::FChargeAttackInfo& Other) const
{
  return FMath::IsNearlyEqual(TimeThreshold, Other.TimeThreshold) && (ChargeAttackTag == Other.ChargeAttackTag);
}
