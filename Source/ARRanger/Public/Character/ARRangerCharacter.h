#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Magnetic/IARMagnetizableInterface.h"
#include "PlayerObservation/IObservableSubjectInterface.h"

#include "BattleSystem/IARAttackerInterface.h"
#include "BattleSystem/IARAttackable.h"

#include "Player/CameraRigType.h"

#include "ARRangerCharacter.generated.h"

/**Forward declaration */
class AARRangerCharacter;
class UAbilitySystemComponent;
class UAttractSpecialAttackComponent;
class UARHealthComponent;
class UARAbilityCostComponent;
class UForceFeedbackEffect;
class ULockOnComponent;
class AInsekiClimbingObject;
class USoundBase;
class UNiagaraSystem;
struct FGameplayTag;

#define UE_API ARRANGER_API

DECLARE_MULTICAST_DELEGATE_TwoParams(FAcceptBattleResultEvent, AARRangerCharacter*, const ARRanger::Battle::FARDamageResult&);
DECLARE_MULTICAST_DELEGATE_OneParam(FBattleStateChangeEvent, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FTransformEvent, EARMagnetismType);
DECLARE_MULTICAST_DELEGATE_OneParam(FExtraTickTask, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCameraRigChanged, ECameraRigType);

/**
 *  シンプルでプレイヤーが操作可能な三人称視点キャラクター
 */
// TODO Maybe we should reduce interface
UCLASS(MinimalAPI, Abstract)
class AARRangerCharacter :  public ACharacter,
                            public IObservableSubjectInterface,
                            public IARMagnetizableInterface,
                            public IARAttackable,               // 攻撃を受けられるインターフェイス
                            public IARAttackerInterface,        // 攻撃できるインターフェイス
                            public IAbilitySystemInterface
{
  GENERATED_BODY()
  
protected:

  /**Start Actor Interface */
  UE_API virtual void BeginPlay() override;
  UE_API virtual void Tick(float DeltaTime) override;
  UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
  /**End Actor Interface */

  // 変身用エフェクトを設定
  UPROPERTY(EditAnywhere, Category = "Effects")
  TObjectPtr<UNiagaraSystem> TransformEffect;

public:

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHitDelegate, FVector, HitLocation);
  
  UPROPERTY(BlueprintAssignable)
  FOnPlayerHitDelegate OnPlayerHit;

  DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadDelegate);
  UPROPERTY(BlueprintAssignable)
  FOnDeadDelegate OnPlayerDead;

  // コンストラクタ
  AARRangerCharacter();

  /**Start ACharacter Interface */
  virtual void Jump() override;
  virtual void StopJumping() override;
  /**End ACharacter Interface */

  FSimpleMulticastDelegate OnJumpedDelegate;
  FSimpleMulticastDelegate OnJumpStoppedDelegate;

  // IAbilitySystemInterface の必須実装
  virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:

  UFUNCTION(BlueprintImplementableEvent, Category = "PlayerCharacter|DeadEvent", meta = (DisplayName = "OnPlayerDeadStarted"))
  UE_API void K2_OnPlayerDeadStarted();

  UFUNCTION(BlueprintImplementableEvent, Category = "PlayerCharacter|DeadEvent", meta = (DisplayName = "OnPlayerDeadFinished"))
  UE_API void K2_OnPlayerDeadFinished();

public:

  virtual void OnClimbStarted();
  virtual void OnClimbUpdated(const FVector& InClimbMovement);
  virtual void OnClimbEnded();

  FExtraTickTask TickTaskDelegate;
  
public:

  // コントロールまたはUIインターフェースからの移動入力を処理する
  UFUNCTION(BlueprintCallable, Category = "Input")
  virtual void DoMove(double InRight, double InForward);

  UFUNCTION(BlueprintCallable, Category = "Input")
  virtual void DoRotate(double InYaw);

  UFUNCTION(BlueprintCallable, Category = "InputCallback")
  void ToggleLockOn();

  UFUNCTION(BlueprintCallable, Category = "InputCallback")
  void SwitchTargetRight();
  
  UFUNCTION(BlueprintCallable, Category = "InputCallback")
  void SwitchTargetLeft();

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Transform", meta = (DisplayName = "OnTransformed"))
  void K2_OnTransformed(EARMagnetismType NewType);

  // 変身の際に呼び出される
  void Transform();

  FTransformEvent OnTransformed;

  // AnimInstanceの戦闘中フラグを設定
  void SetIsBattledInAnimInstance(const bool IsBattled);

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Battle", meta = (DisplayName = "OnBattleStarted"))
  void K2_OnBattleStarted();

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Battle", meta = (DisplayName = "OnBattleEnded"))
  void K2_OnBattleEnded();

  void UpdateTargetSnap(const FVector2D& InputDir);

  UE_API bool TryApplyAbilityCost(const FGameplayTag& InAbilityCostTag, float InAbilictCostChangeNum);

  // ロックオンコンポーネント
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TObjectPtr<ULockOnComponent> LockOnComponent;

  // 変身時のサウンド
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
  TObjectPtr<USoundBase> SE_Transform;

  UE_API void LaunchCharacter_Ext(const FVector& InLaunchDirection, double InLaunchPower);

  UFUNCTION(BlueprintImplementableEvent, Category = "Character|HealthEffect")
  UE_API void OnVignetteEffectChanged(UARHealthComponent* InHealthComponent, AActor* InInstigator, float InOldHealthValue, float InNewHealthValue);

  // 現在のプレイヤーのモードを取得
  UFUNCTION(BlueprintPure)
  EARMagnetismType GetCurrentARType() const;

  // 攻撃フラグをリセット
  UFUNCTION(BlueprintCallable)
  void ResetIsAttacked();

  // ロックオンフラグを取得
  bool GetIsLockedOn() const;

  // 引き寄せ完了フラグをセット
  void SetIsApproachedEnemy(bool IsApproachedEnemy) { bIsApproachedEnemy = IsApproachedEnemy; }

  // 引き寄せ完了フラグを取得
  bool GetIsApproachedEnemy() const { return bIsApproachedEnemy; }

  // 引き寄せ完了時に呼びだされる関数
  void OnAttractionCompleted();
  
  // コンボ受付フラグをセット
  void SetInComboWindow(bool bIn) { bIsInComboWindow = bIn; }

  // コンボ受付フラグを取得
  bool GetIsInComboWindow() const { return bIsInComboWindow; }

  // コンボカウントを加算
  void AddComboCount() { ++ComboCount; }

  // コンボカウントをリセット
  void ResetComboCount() { ComboCount = 0; }

  // コンボカウントを取得
  int32 GetComboCount() const { return ComboCount; }

  // AttackComponent内で使用するNotifyHandler用
  void OnAttackHitNotify();

  // 必殺技時に呼び出される
  UFUNCTION(BlueprintCallable)
  void OnSpecialAttractAttack();

  virtual void OnLanded(const FHitResult& Hit);

  // 麦
  UFUNCTION(BlueprintCallable, Category = "GameAbility|Callbacks")
  void OnPunchStarted();

  UFUNCTION(BlueprintCallable, Category = "GameAbility|Callbacks")
  void OnPunchEnded();

  UFUNCTION(BlueprintCallable, Category = "GameAbility|Callbacks")
  virtual void OnAttackAbilityStarted();

  UFUNCTION(BlueprintCallable, Category = "GameAbility|Callbacks")
  virtual void OnAttackAbilityEnded();
 
  UFUNCTION(BlueprintCallable, Category = "Camera")
  void SetCameraRig(ECameraRigType InType);

  UFUNCTION(BlueprintPure, Category = "Camera")
  ECameraRigType GetCameraRig() const;
  
  FOnCameraRigChanged CameraRigChangeEvent;

private:
  
  ECameraRigType CameraRigType;
  
  UFUNCTION()
  UE_API void OnPlayerDeadStarted(AActor* PlayerActor);

  UFUNCTION()
  UE_API void OnPlayerDeadEnded(AActor* PlayerActor);

  UFUNCTION()
  UE_API void OnAbilityCostHandled(UARAbilityCostComponent* InAbilityCostComponent, FGameplayTag AbilityCostTag, float InOldResourceValue, float InNewResourceValue, bool bAbilityCostHandled);

  void DisableMovementAndCollision();

  void PlayTransformEffect();
  void TransformInternal();

// NOTE: Prepare for MVP pattern
public:

  FAcceptBattleResultEvent OnBattleResultAccepted;

  FBattleStateChangeEvent OnBattleStateChanged;

  void OnHealthChanged(AActor* InInstigator, float InChangeValue, bool bIsDead);

protected:
  UFUNCTION(BlueprintImplementableEvent, Category = "Character|Health")
  void K2_OnHealthChanged(AActor* InInstigator, float InChangeValue, bool bIsDead);

private:

  // 敵引き寄せ完了フラグ
  bool bIsApproachedEnemy = false;

  // コンボ受付中かどうか
  bool bIsInComboWindow = false;

  // コンボカウント
  int32 ComboCount = 0;

  // 必殺技コンポーネントを取得
  UPROPERTY()
  TObjectPtr<UAttractSpecialAttackComponent> attractSpecialAttackComponent = nullptr;

  UPROPERTY(EditDefaultsOnly, Category = "Character|Parameters", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UARHealthComponent> HealthComponent;

  UPROPERTY(EditDefaultsOnly, Category = "Character|Parameters", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UARAbilityCostComponent> AbilityCostComponent;

  /**Controler vibration */
  public:
  // 一定の落下時間を設定(落下時間がこの値を超えると着地時に振動が発生)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
  float MinFallTimeForFeedback = 0.0f;

  // 攻撃時のフォースフィードバックエフェクトを設定
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feedback")
  TObjectPtr<UForceFeedbackEffect> FFE_Attack;

  // 着地時のフォースフィードバックエフェクトを設定
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Feedback")
  TObjectPtr<UForceFeedbackEffect> FFE_Landed;

  // TODO Use to snap target when player is in punch state
  FVector2D TargetSnapInput;

  bool bReadyToTargetSnap;

  bool bCanTargetSnap;

  UPROPERTY()
  TObjectPtr<AActor> TargetToSnap;

  UPROPERTY()
  TObjectPtr<UPrimitiveComponent> TargetPrimitiveComp;

  FVector TargetImpactPoint_Local;

  UPROPERTY(EditAnywhere, Category = "ARRanger|TargetSnap")
  float TargetSnapDetectLength;

  UPROPERTY(EditAnywhere, Category = "ARRanger|TargetSnap")
  float SnapTimeInterval = 0.2f;

  float m_snapTimeCnt = 0.0f;

  FVector m_startSnapPlayerLocation;
  FRotator m_startSnapPlayerRotation;

  void SearchTargetToSnap();

  void SnapToTarget(float DeltaTime);
  // TODO End

  // 必殺技を使用可能かを返す関数
  bool CanSpecialAttractAttack();

  /**Start IARMagnetizableInterface interface */
  UE_API virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) override;
  UE_API virtual AActor* GetActor() override { return this; }
  /**End IARMagnetizableInterface interface */

  /**Start IARAttackable Interface */
protected:
  UE_API virtual AActor* Attackable_GetActor() override { return this; }
  UE_API virtual bool CanAttack() override;
  UE_API virtual void OnPreAttacked(const FARAttackParameters& InAttackParams, ARRanger::Battle::FARAttackResult& OutAttackResult) override;
  UE_API virtual void OnPostAttacked(const FARAttackParameters& InAttackParams) override;
  UE_API virtual void OnDamaged(const ARRanger::Battle::FARDamageResult& InDamageResult) override;
  /**End IARAttackable Interface */

  /**Start IARAttackerInterface Interface */
  UE_API virtual void OnNotifyAttackResult_Success(const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams) override;
  /**End IARAttackerInterface Interface */
};

#undef UE_API