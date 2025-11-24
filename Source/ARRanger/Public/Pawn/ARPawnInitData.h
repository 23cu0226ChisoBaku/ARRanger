/**
 * @file ARPawnInitData.h
 * @brief ポーン初期化アセット
 */

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "ARPawnInitData.generated.h"

#define UE_API ARRANGER_API

/**前方宣言 */
class UARGameplayAbilityBase;

/**
 * @struct FChargeAttackLeaf
 * @brief Leaf data of charge attack
 */
USTRUCT(BlueprintType)
struct FChargeAttackLeaf
{
  GENERATED_BODY()

  /**Charge time threshold */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  float TimeThreshold;

  /**Charge attack cost */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  float ChargeAttackCost;

  /**Charge attack ability tag */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FGameplayTag AttackTag;
};

/**
 * @struct FChargeAttackBranchEntry
 * @brief  Charge tree branch to keep leaves.(@see FChargeAttackLeaf)
 */
USTRUCT(BlueprintType)
struct FChargeAttackBranchEntry
{
  GENERATED_BODY()

  /**Branch tag. Also be used as charge ability tag */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  FGameplayTag BranchTag;

  /**Branch leaves */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
  TSet< FChargeAttackLeaf > BranchLeaves;
};

/**
 * @class UARPawnInitData
 * @brief Asset to initialize pawn in ARPawnInitComponent
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Pawn Init Data"))
class UARPawnInitData : public UDataAsset
{
	GENERATED_BODY()
	
public:
  UE_API UARPawnInitData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

  /**GameplayAbilities to initialize */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayAbility")
  TSet< TSoftClassPtr< UARGameplayAbilityBase > > Abilities;

  /**Charge attack branch entries */
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayAbility", meta = (TitleProperty = "Charge Branch Tag: {BranchTag}."))
  TArray< FChargeAttackBranchEntry > ChargeBranchEntries;
};

/**Define to use these in TArray and TSet */
extern UE_API bool operator==(const FChargeAttackLeaf& Lhs, const FChargeAttackLeaf& Rhs);
extern UE_API bool operator==(const FChargeAttackBranchEntry& Lhs, const FChargeAttackBranchEntry& Rhs);

/**Define to use these in TArray and TSet */
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