// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/ARPawnInitData.h"

UARPawnInitData::UARPawnInitData(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  Abilities.Reset();
  AttributeSets.Reset();
  ChargeBranchEntries.Reset();
}

bool operator==(const FChargeAttackLeaf& Lhs, const FChargeAttackLeaf& Rhs)
{
  return (Lhs.TimeThreshold == Rhs.TimeThreshold) && (Lhs.AttackTag == Rhs.AttackTag);
}

bool operator==(const FChargeAttackBranchEntry& Lhs, const FChargeAttackBranchEntry& Rhs)
{
  // Make Branch as an unique sign
  return (Lhs.BranchTag == Rhs.BranchTag);
}

#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FChargeAttackLeaf& Element)
{
  uint32 Hash = FCrc::MemCrc32(&Element, sizeof(FChargeAttackLeaf));
  return Hash;
}

uint32 GetTypeHash(const FChargeAttackBranchEntry& Element)
{
  uint32 Hash = GetTypeHash(Element.BranchTag);
  for (const FChargeAttackLeaf& leaf : Element.BranchLeaves)
  {
    Hash = HashCombine(Hash, GetTypeHash(leaf));
  }

  return Hash;
}
#endif