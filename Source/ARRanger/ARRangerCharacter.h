#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h" 
#include "GameFramework/Character.h"
#include "IARMagnetizableInterface.h"
#include "InsekiClimbingObject.h"
#include "LockOnComponent.h"
#include "Logging/LogMacros.h"
#include "NiagaraSystem.h"
#include "Physics/IARPhysicsSystemHost.h"
#include "PlayerObservation/IObservableSubjectInterface.h"
#include "Sound/SoundBase.h"

#include "BattleSystem/IARAttackerInterface.h"
#include "BattleSystem/IARAttackable.h"


#include "ARRangerCharacter.generated.h"

class UAttackBaseComponent;
class UAbilitySystemComponent;
class UAnimMontage;
class USkeletalMesh;
class UAttractSpecialAttackComponent;
class UARHealthComponent;
class UARAbilityCostComponent;
class UForceFeedbackEffect;

struct FGameplayTag;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

#define UE_API ARRANGER_API

/**
 *  シンプルでプレイヤーが操作可能な三人称視点キャラクター
 *  制御可能な軌道カメラの実装
 */
// TODO Maybe we should reduce interface
UCLASS(Abstract)
class AARRangerCharacter :  public ACharacter,
                            public IObservableSubjectInterface,
                            public IARMagnetizableInterface,
                            public IARPhysicsSystemHost,
                            public IARAttackable,               // 攻撃を受けられるインターフェイス
                            public IARAttackerInterface,        // 攻撃できるインターフェイス
                            public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	protected:

  virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	// 麦
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 引力クライムフラグ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsClimbed;

	// 変身用エフェクトを設定
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* TransformEffect;

public:

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHitDelegate, FVector, HitLocation);
  
  UPROPERTY(BlueprintAssignable)
  FOnPlayerHitDelegate OnPlayerHit;

  DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadDelegate);
  UPROPERTY(BlueprintAssignable)
  FOnDeadDelegate OnPlayerDead;

	// コンストラクタ
	AARRangerCharacter();

	// IAbilitySystemInterface の必須実装
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:

	// 入力アクションのバインディングを初期化する
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

  
  UFUNCTION(BlueprintImplementableEvent, Category = "PlayerCharacter|DeadEvent", meta = (DisplayName = "OnPlayerDeadStarted"))
  UE_API void K2_OnPlayerDeadStarted();

  UFUNCTION(BlueprintImplementableEvent, Category = "PlayerCharacter|DeadEvent", meta = (DisplayName = "OnPlayerDeadFinished"))
  UE_API void K2_OnPlayerDeadFinished();

private:

	// 現在歩いているオブジェクトの表面
	UPROPERTY()
	TObjectPtr<AInsekiClimbingObject> currentClimbSurface;

	// 壁の法線を保存
	FVector wallNormal;

	// 引力クライムオブジェクトに触れた際に呼び出される
  UFUNCTION()
	void OnClimbSurfaceOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// 引力クライムを開始する際に呼び出される
	void StartClimbing(AInsekiClimbingObject* ClimbActor);

	// 引力クライムをやめる際に呼び出される
	void StopClimbing();

public:

	// コントロールまたはUIインターフェースからの移動入力を処理する
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	// コントロールまたはUIインターフェースからのルック入力を処理する
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	// コントロールまたはUIインターフェースのどちらからでも、押されたジャンプ入力を処理する
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	// コントロールまたはUIインターフェースのどちらからでも、押されたジャンプ入力を処理する
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

  UFUNCTION(BlueprintCallable, Category = "InputCallback")
  void ToggleLockOn();

  UFUNCTION(BlueprintCallable, Category = "InputCallback")
  void SwitchTargetRight();
  
  UFUNCTION(BlueprintCallable, Category = "InputCallback")
  void SwitchTargetLeft();

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Transform", meta = (DisplayName = "OnTransformed"))
  void K2_OnTransformed(USkeletalMesh* NewTransformedMesh, EARMagnetismType NewType);

  // 変身の際に呼び出される
  void Transform();

  void ResetCamera();

  // AnimInstanceの戦闘中フラグを設定
  void SetIsBattledInAnimInstance(const bool IsBattled);

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Battle", meta = (DisplayName = "OnBattleStarted"))
  void K2_OnBattleStarted();

  UFUNCTION(BlueprintImplementableEvent, Category = "ARRanger|Battle", meta = (DisplayName = "OnBattleEnded"))
  void K2_OnBattleEnded();

  void RotateCharacter_Charge(float Yaw);

  void UpdateTargetSnap(const FVector2D& InputDir);

  // Call if we start charge kick
  void OnHoldStarted(const FGameplayTag& InActivatedAbilityTag);
  void OnHoldEnded();

  UE_API bool TryApplyAbilityCost(const FGameplayTag& InAbilityCostTag, float InAbilictCostChangeNum);

	// 引力用プレイヤーメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerMesh")
	USkeletalMesh* AttractionMesh;

	// 斥力用プレイヤーメッシュ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerMesh")
	USkeletalMesh* RepulsionMesh;

	// ダッシュ中フラグ
	UPROPERTY(BlueprintReadWrite)
	bool bIsDashed;

	// ロックオンコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ULockOnComponent> LockOnComponent;

	// 変身時のサウンド
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> SE_Transform;

public:
	virtual void Tick(float DeltaTime) override;

  // 麦
  UFUNCTION(BlueprintImplementableEvent, Category = "Character|HealthEffect")
  UE_API void OnVignetteEffectChanged(UARHealthComponent* InHealthComponent, AActor* InInstigator, float InOldHealthValue, float InNewHealthValue);

	// AttackBaseComponentを保存
  UPROPERTY()
	TObjectPtr<UAttackBaseComponent> AttackBaseComp = nullptr;

	// 移動時のデッドゾーン(下回ると移動しない)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	float MoveDeadZone = 0.15f;

	// 移動時インプットの最低値(デッドゾーンを上回っている際の最低値)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	float MinInput = 0.3f;

	// 現在のプレイヤーのモードを取得
	UFUNCTION(BlueprintPure)
	EARMagnetismType GetCurrentARType();

	// 攻撃フラグをリセット
	UFUNCTION(BlueprintCallable)
	void ResetIsAttacked();

	// 攻撃中フラグを取得
	bool GetIsAttacked() { return bIsAttacked; }

	// 攻撃中フラグをセット
	void SetIsAttacked(bool IsAttacked) { bIsAttacked = IsAttacked; }

	// 強攻撃中フラグをセット
	void SetIsStrongAttacked(bool IsStrongAttacked) { bIsStrongAttacked = IsStrongAttacked; }

	// ロックオンフラグを取得
	bool GetIsLockedOn() { return LockOnComponent->GetIsLockedOn(); }

	// 引き寄せ中フラグをセット
	void SetIsAttracted(bool IsAttracted) { bIsAttracted = IsAttracted; }

	// 引き寄せ中フラグを取得
	bool GetIsAttracted() { return bIsAttracted; }

	// 引き寄せ完了フラグをセット
	void SetIsApproachedEnemy(bool IsApproachedEnemy) { bIsApproachedEnemy = IsApproachedEnemy; }

	// 引き寄せ完了フラグを取得
	bool GetIsApproachedEnemy() { return bIsApproachedEnemy; }

	// 引き寄せ完了時に呼びだされる関数
	void OnAttractionCompleted();

	// 敵死亡時に呼び出される関数
	void OnDeadEnemy();

	// コンボ受付フラグをセット
	void SetInComboWindow(bool bIn) { bIsInComboWindow = bIn; }

	// コンボ受付フラグを取得
	bool GetIsInComboWindow() const { return bIsInComboWindow; }

	// コンボカウントを加算
	void AddComboCount() { ++ComboCount; }

	// コンボカウントをリセット
	void ResetComboCount() { ComboCount = 0; }

	// コンボカウントを取得
	int32 GetComboCount() { return ComboCount; }

	// 引力クライム中フラグを取得
	bool GetIsClimbed() { return bIsClimbed; }

	// AttackComponent内で使用するNotifyHandler用
	void OnAttackHitNotify();

	// 必殺技時に呼び出される
	UFUNCTION(BlueprintCallable)
	void OnSpecialAttractAttack();

	// 麦
	bool bIsJumping = false;

	UFUNCTION()
	void LandedToGround(const FHitResult& Hit)
	{
		bIsJumping = false;
	}

  // 麦
  UFUNCTION(BlueprintCallable, Category = "GameAbility|Callbacks")
  void OnPunchStarted();

  UFUNCTION(BlueprintCallable, Category = "GameAbility|Callbacks")
  void OnPunchEnded();

private:
  UFUNCTION()
  UE_API void OnPlayerDeadStarted(AActor* PlayerActor);

  UFUNCTION()
  UE_API void OnPlayerDeadEnded(AActor* PlayerActor);


  UFUNCTION()
  UE_API void OnAbilityCostHandled(UARAbilityCostComponent* InAbilityCostComponent, FGameplayTag AbilityCostTag, float InOldResourceValue, float InNewResourceValue, bool bAbilityCostHandled);

  void DisableMovementAndCollision();

private:
	// 攻撃中フラグ
	bool bIsAttacked = false;

	// 強攻撃中フラグ
	bool bIsStrongAttacked = false;

	// 引き寄せ中フラグ
	bool bIsAttracted = false;

	// 敵引き寄せ完了フラグ
	bool bIsApproachedEnemy = false;

	// コンボ受付中かどうか
	bool bIsInComboWindow = false;

	// コンボカウント
	int32 ComboCount = 0;

	// 必殺技コンポーネントを取得
	UPROPERTY()
	UAttractSpecialAttackComponent* attractSpecialAttackComponent = nullptr;

  UPROPERTY(EditDefaultsOnly, Category = "Character|Parameters", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UARHealthComponent> HealthComponent;

  UPROPERTY(EditDefaultsOnly, Category = "Character|Parameters", meta = (AllowPrivateAccess = "true"))
  TObjectPtr<UARAbilityCostComponent> AbilityCostComponent;

  UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
  int32 CameraRigIndex;

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

  // TODO Use to rotate when player is in charge state
  FVector FaceDir_HoldStart;

  bool bIsHolding;

  float OffsetDegreeFromHoldStartDir;
  // TODO End

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

	UFUNCTION()
	void OnMagneticForceFieldBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMagneticForceFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnMagnetizedObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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