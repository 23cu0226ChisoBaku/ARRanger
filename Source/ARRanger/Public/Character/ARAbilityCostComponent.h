/**
 * @file ARAbilityCostComponent.h
 * @author MAI ZHICONG
 * @brief アビリティコストを保存するコンポーネント
 */

#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "ARAbilityCostComponent.generated.h"

#define UE_API ARRANGER_API

class UARAbilityCostComponent;

USTRUCT(BlueprintType, Blueprintable)
struct FAbilityCostEntry
{
  GENERATED_BODY()

  /**アビリティコスト資源Tag*/
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityCost")
  FGameplayTag AbilityCostTag;

  /**コスト資源の数 */
  UPROPERTY(VisibleAnywhere, Category = "AbilityCost")
  float ResourceNum;

  /**コスト資源の最大数 */
  UPROPERTY(EditDefaultsOnly, Category = "AbilityCost")
  float MaxResourceNum;

  DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCostChangedDelegate, float, float, bool);

  /**コスト資源数が変更するとき呼び出されるデリゲート */
  FOnAbilityCostChangedDelegate OnAbilityCostChanged;

  UE_API float GetResourceNum() const;
  UE_API float GetMaxResourceNum() const;
  UE_API void SetResourceNum(float NewResourceNum, bool& bOutResult);
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UARAbilityCostComponent : public UActorComponent
{
	GENERATED_BODY()

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnAbilityCostAppliedDynamicDelegate, UARAbilityCostComponent*, AbilityCostComponent, FGameplayTag, AbilityCostTag, float, OldResourceNum, float, NewResourceNum, bool, bIsAbilityCostApplied);
  
  using AbilityCostDelegate_Native = FAbilityCostEntry::FOnAbilityCostChangedDelegate::FDelegate;
  
public:	
  UPROPERTY(BlueprintAssignable)
  FOnAbilityCostAppliedDynamicDelegate OnAbilityCostApplied;

	UE_API UARAbilityCostComponent();

  UE_API void HandleAbilityCostChanged(const FGameplayTag& InAbilityCostTag, float InResourceChangeNum, bool& bOutResult);
  UE_API [[nodiscard]] FDelegateHandle AddDelegateToAbilityCost(const FGameplayTag& InAbilityCostTag, AbilityCostDelegate_Native&& InDelegate);
  UE_API void RemoveDelegateInAbilityCost(const FGameplayTag& InAbilityCostTag, FDelegateHandle InHandle);

private:

  UE_API [[nodiscard]] FAbilityCostEntry* FindEntryByTag(const FGameplayTag& InAbilityCostTag);

private:

  UPROPERTY(EditDefaultsOnly, Category = "AbilityCost|Units", meta = (AllowPrivateAccess = "true"))
  TArray<FAbilityCostEntry> AbilityCostEntries;

};

#undef UE_API