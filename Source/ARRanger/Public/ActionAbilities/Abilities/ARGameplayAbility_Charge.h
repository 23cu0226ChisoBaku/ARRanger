/**
 * @file ARGameplayAbility_Charge.h
 * @author MAI ZHICONG
 * @brief 溜めアクション専用アビリティ
 */

#pragma once

#include "ActionAbilities/ARGameplayAbilityBase.h"

#include "ARGameplayAbility_Charge.generated.h"

#define UE_API ARRANGER_API

/**前方宣言 */
class UAbilityTask_WaitGameplayTagAdded;

UCLASS()
class UARGameplayAbility_Charge : public UARGameplayAbilityBase
{
  GENERATED_BODY()
  
public:
  UE_API UARGameplayAbility_Charge();
  
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnChargeFinishedDelegate, bool, bWasCanceled, float, TimeHeld, FGameplayTagContainer, ChargeAbilityTags);
  
  /**溜めアクションが終了するとき呼び出されるデリゲート */
  UPROPERTY(BlueprintAssignable)
  FOnChargeFinishedDelegate OnChargeFinished;

protected:

  /**Start UGameplayAbility Interface */
  UE_API virtual void ActivateAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          const FGameplayEventData* TriggerEventData
                      ) override;
  UE_API virtual void EndAbility(
                          const FGameplayAbilitySpecHandle Handle,
                          const FGameplayAbilityActorInfo* ActorInfo,
                          const FGameplayAbilityActivationInfo ActivationInfo,
                          bool bReplicateEndAbility, bool bWasCancelled
                      ) override;
  UE_API virtual void InputReleased(
                          const FGameplayAbilitySpecHandle Handle, 
                          const FGameplayAbilityActorInfo* ActorInfo, 
                          const FGameplayAbilityActivationInfo ActivationInfo
                      ) override;
  /**End UGameplayAbility Interface */

  /**
   * @brief 溜めアビリティの持続時間を取得
   * 
   * @return 持続時間（無効な場合は0.0f）
   */
  UFUNCTION(BlueprintPure, Category = "Ability|ChargeInternal")
  float GetHeldTime() const;

private:

  UFUNCTION()
  void OnCancelableTagAdded();
  UFUNCTION()
  void OnEndableTagAdded();

  /**
   * @brief 溜めタスクを生成
   */
  void GenerateTasks();

  /**
   * @brief 溜めタスクを破棄
   */
  void DiscardTasks();

private:

  // TODO Maybe move this to base class?
  /**溜めアビリティモンタージュ */
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ChargeMontage;

  /**溜めをキャンセルできるTags */
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true", DisplayName = "Cancel Tags When Added"))
  FGameplayTagContainer CancelableTags;

  /**溜めを終わらせられるTags */
  UPROPERTY(EditDefaultsOnly, Category = "Ability|Charge", meta = (AllowPrivateAccess = "true", DisplayName = "End Tags When Added"))
  FGameplayTagContainer EndableTags;
  
  /**キャンセルタスク */
  UPROPERTY()
  TArray< TObjectPtr< UAbilityTask_WaitGameplayTagAdded > > CancelTasks;

  /**終了タスク */
  UPROPERTY()
  TArray< TObjectPtr< UAbilityTask_WaitGameplayTagAdded > > EndTasks;

  /**溜めアビリティ開始時間 */
  float m_startTime;

};

#undef UE_API