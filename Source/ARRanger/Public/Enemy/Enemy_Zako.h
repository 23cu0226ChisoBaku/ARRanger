#pragma once

#include "GameFramework/Character.h"
#include "BattleSystem/IARAttackable.h" 
#include "BattleSystem/IARAttackerInterface.h"
#include "ISpecialAttractInterface.h"

#include "Enemy_Zako.generated.h"

#define UE_API ARRANGER_API

class UARHealthComponent;

UCLASS()
class AEnemy_Zako : public ACharacter, 
                    public IARAttackable,
                    public IARAttackerInterface,
                    public ISpecialAttractInterface
{
  GENERATED_BODY()

public:
  
  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeadEvent, AActor*, DeadActor);
  UPROPERTY(BlueprintAssignable)
  FOnEnemyDeadEvent OnEnemyDeadStarted;

  UPROPERTY(BlueprintAssignable)
  FOnEnemyDeadEvent OnEnemyDeadEnded;

public:
  UE_API AEnemy_Zako();

  UE_API void SetIsChasing(bool bChasing);

  /**Start AActor Interface */
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  UE_API virtual void Destroyed() override;
  /**End AActor Interface */

protected:
  UE_API virtual void ReceiveDamage(AActor* InInstigator, float DamageAmount);

  UE_API virtual void ReceiveLaunch(const FVector& LaunchDirection);

  UE_API virtual void PerformDeadStartEffect();

  UE_API virtual void PerformDeadEndEffect();

public:
  UE_API virtual void Zako_PerformAttack();

  UFUNCTION(BlueprintImplementableEvent, Category = "Enemy", meta = (DisplayName = "Perform Attack"))
  UE_API void K2_PerformAttack();

  UFUNCTION(BlueprintImplementableEvent, Category = "Enemy", meta = (DisplayName = "Receive Attack"))
  UE_API void K2_ReceiveAttack(float DamageAmount, FVector LaunchDirection, FVector ImpactLocation, bool bIsDead);


protected:
  UE_API virtual bool IsDead() const; 

public:

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
  bool isDead;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
  float PreferredDistance = 20.0f; 

  // 引き寄せ開始関数
  void StartAttraction(AActor* Target);

private:
  // 引き寄せられ中フラグ
  bool bIsAttracted = false;

protected:
  bool bDeadLaunchHandled = false;

  // 引き寄せられる対象(プレイヤー)
  UPROPERTY()
  TObjectPtr<AActor> attractionTarget = nullptr;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Health", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UARHealthComponent> HealthComponent;

  FTimerHandle StartDeadTimer;

  // 引き寄せ停止関数
  void StopAttraction();

  UFUNCTION()
  void EnemyDeadStarted(AActor* OwningActor);

  UFUNCTION()
  void EnemyDeadFinished(AActor* OwningActor);
  
  UFUNCTION()
  void OnEnemyHealthChanged(UARHealthComponent* InHealthComponent, AActor* InInstigator, float PreviousHealth, float CurrentHealth);

protected:
  // 引き寄せ時のスピード
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
  float attractionSpeed = 800.f;

  // 引き寄せの最低距離(これ以下になったらパンチ開始)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
  float MinDistance = 150.f;

  UE_API virtual bool CanAttack() override;

  UE_API virtual void OnPreAttacked(
      const FARAttackParameters& InAttackParams,
      ARRanger::Battle::FARAttackResult& OutAttackResult) override;

  UE_API virtual void OnDamaged(
      const ARRanger::Battle::FARDamageResult& InDamageResult) override;

  UE_API virtual void OnPostAttacked(
      const FARAttackParameters& InAttackParams) override;

  /**Start ISpecialAttractInterface Interface */

  //引力必殺技が始まった時の通知
  UE_API virtual void OnStartSpecialAttractNotify() override;

  //brief 引力必殺技の中間通知
  //param 経過時間
  UE_API virtual void OnUpdateSpecialAttractNotify(float ElapsedTime) override;

  //brief 引力必殺技の終了通知
  UE_API virtual void OnEndSpecialAttractNotify() override;
  
  /**End ISpecialAttractInterface Interface */
};

#undef UE_API