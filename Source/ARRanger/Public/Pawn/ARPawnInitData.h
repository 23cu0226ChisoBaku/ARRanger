// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "ARPawnInitData.generated.h"

#define UE_API ARRANGER_API

class UARGameplayAbilityBase;
class UARAttributeSet;

USTRUCT(BlueprintType)
struct FChargeAttackLeaf
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  float TimeThreshold;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  float ChargeAttackCost;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FGameplayTag AttackTag;
};

USTRUCT(BlueprintType)
struct FChargeAttackBranchEntry
{
  GENERATED_BODY()

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FGameplayTag BranchTag;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TSet< FChargeAttackLeaf > BranchLeaves;

};

/**
 * 
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Pawn Init Data"))
class UARPawnInitData : public UDataAsset
{
	GENERATED_BODY()
	
public:
  UE_API UARPawnInitData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayAbility")
  TSet< TSoftClassPtr< UARGameplayAbilityBase > > Abilities;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayAbility")
  TSet< TSoftClassPtr<UARAttributeSet> > AttributeSets;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayAbility", meta = (TitleProperty = "Charge Branch Tag: {BranchTag}."))
  TArray< FChargeAttackBranchEntry > ChargeBranchEntries;
};

extern UE_API bool operator==(const FChargeAttackLeaf& Lhs, const FChargeAttackLeaf& Rhs);
extern UE_API bool operator==(const FChargeAttackBranchEntry& Lhs, const FChargeAttackBranchEntry& Rhs);

#if UE_BUILD_DEBUG
extern UE_API uint32 GetTypeHash(const FChargeAttackLeaf& Element);
extern UE_API uint32 GetTypeHash(const FChargeAttackBranchEntry& Element);
#else
__forceinline uint32 GetTypeHash(const FChargeAttackLeaf& Element)
{
  uint32 Hash = FCrc::MemCrc32(&Element, sizeof(FChargeAttackLeaf));
  return Hash;
}

__forceinline uint32 GetTypeHash(const FChargeAttackBranchEntry& Element)
{
  uint32 Hash = GetTypeHash(Element.BranchTag);
  for (const FChargeAttackLeaf& leaf : Element.BranchLeaves)
  {
    Hash = HashCombine(Hash, GetTypeHash(leaf));
  }

  return Hash;
}
#endif // UE_BUILD_DEBUG

#undef UE_API