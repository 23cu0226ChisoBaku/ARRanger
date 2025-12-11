/**
 * @file ARHealthComponent.h
 * @author MAI ZHICONG
 * @brief HP処理をするコンポーネント
 */

#pragma once

#include "Components/ActorComponent.h"

#include "ARHealthComponent.generated.h"

#define UE_API ARRANGER_API

/**前方宣言 */
class UCurveFloat;

/**
 * @brief HP回復情報を持つ構造体
 */
USTRUCT(BlueprintType)
struct FARHealthRegenerationEntry
{
  GENERATED_BODY()

public:

  /**回復遅延（時間） */
  UPROPERTY(EditDefaultsOnly)
  float RegenerationDelay;

  /**回復速度（DeltaTimeベース） */
  UPROPERTY(EditDefaultsOnly)
  float RegenerationSpeed;

  /**回復速度をCurveで表すか */
  UPROPERTY(EditDefaultsOnly)
  bool bUseRegenerationSpeedOverrideCurve;

  /**回復速度カーブ */
  UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bUseRegenerationSpeedOverrideCurve == true", EditConditionHides))
  TSoftObjectPtr<UCurveFloat> OverrideCurve;

  /**カーブを評価する時間 */
  float RegenerationCurveEvaluationTimeValue = 0.0f;

  /**回復遅延カウンター */
  float RegenerationDelayTimeCnt = 0.0f;
  
  /**回復が有効か */
  uint8 bEnableRegeneration : 1;

  /**
   * @brief 回復状態をリセットする
   */
  UE_API void ResetRegenerationState();

  /**
   * @brief 回復を評価する
   * @param DeltaTime 
   */
  UE_API void EvaluateRegeneration(float DeltaTime);

  /**
   * @brief 回復速度を取得 
   */
  UE_API float GetRegenerationSpeed() const;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UARHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UE_API UARHealthComponent();

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedDelegate, UARHealthComponent*, HealthComponent, AActor*, Instigator, float, OldHealthValue, float, NewHealthValue);
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeadDelegate, AActor*, OwningActor);

  UPROPERTY(BlueprintAssignable)
  FOnHealthChangedDelegate OnHealthChanged;

  UPROPERTY(BlueprintAssignable)
  FOnDeadDelegate OnDeadEventStarted;

  UPROPERTY(BlueprintAssignable)
  FOnDeadDelegate OnDeadEventFinished;

  UFUNCTION(BlueprintCallable, Category = "ARRanger|Health")
  static UE_API UARHealthComponent* FindHealthComponent(AActor* OwningActor);

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetHealth() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetMaxHealth() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API float GetHealthNormalized() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API bool IsDead() const;

  UFUNCTION(BlueprintPure, Category = "ARRanger|Health")
  UE_API bool IsHealthMax() const;

  /**
   * @brief HP変化請求を処理する
   * 
   * @param Instigator HPへ影響を与えるターゲットActor
   * @param ChangeValue HPの変化量
   */
  UE_API void HandleHealthChange(AActor* Instigator, float ChangeValue);

  UE_API void StartDead();

  UE_API void FinishDead();

  /**
   * @brief 自動回復の有効化を設定
   * 
   * @param bEnable 
   */
  UE_API void SetAutoRegenerationEnable(const bool bEnable);

  /**TODO Temporary variable */
  bool bAutoRegenerationEnable = true;
  /** */

protected:

  /**Start UActorComponent Interface */
	UE_API virtual void BeginPlay() override;
  UE_API virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  /**End UActorComponent Interface */

private:
  void SetHealthInternal(float NewHealth);
  void SetMaxHealthInternal(float NewMaxHealth);
  /**
   * @brief HPが0になった時の処理
   * 
   * @param OwningActor HPコンポーネントを持っているActor
   */
  void HandleOutOfHealth(AActor* OwningActor);

private:
  UPROPERTY(EditAnywhere, Category = "ARRanger|Health", meta = (AllowPrivateAccess = "true"))
  float MaxHealth;

  UPROPERTY(VisibleAnywhere, Category = "ARRanger|Health")
  float Health;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Health", meta = (AllowPrivateAccess = "true"))
  bool bAutoRegeneration;

  UPROPERTY(EditDefaultsOnly, Category = "ARRanger|Health", meta = (EditCondition = "bAutoRegeneration == true", EditConditionHides))
  FARHealthRegenerationEntry RegenerationEntry;
		
};

#undef UE_API