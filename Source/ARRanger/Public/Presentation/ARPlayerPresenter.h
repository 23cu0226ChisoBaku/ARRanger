/**
 * @file ARPlayerPresenter.h
 * @author MAI ZHICONG
 * @brief プレイヤーのデータとビューを制御するプレゼンター（MVP）
 */

#pragma once

#include "UObject/NoExportTypes.h"
#include "Player/CameraRigType.h"

#include "Physics/IARPhysicsSystemHost.h"

#include "ARPlayerPresenter.generated.h"

/**前方宣言 */
class AARRangerCharacter;
class APlayerController;
class UARHealthComponent;
class ULockOnComponent;
class UCameraRouterComponent;
enum class ECameraRigType : uint8;
enum class EARMagnetismType : uint8;

namespace ARRanger
{

namespace Battle
{
  struct FARDamageResult;
} // namespace ARRanger::Battle

} // namespace ARRanger

#define UE_API ARRANGER_API

USTRUCT()
struct FARPlayerModel
{
  GENERATED_BODY()

  DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCameraRigChanged, ECameraRigType, ECameraRigType);
  DECLARE_MULTICAST_DELEGATE_OneParam(FOnLockOnTargetUpdated, AActor*);

private:
  enum ESwitchTargetState : uint8
  {
    None,
    Left,
    Right,
  };

public:
  FARPlayerModel();

  /**
   * @brief InViewCharacterのコンポーネントを取得して初期化する
   * @param InViewCharacter プレイヤーキャラクター
   */
  void Initialize(AARRangerCharacter* InViewCharacter);
  void Reset();

  void SetCameraRig(ECameraRigType Type);

  void UpdateLockOnTargets(AActor* UserActor, const TArray<AActor*>& InTargets);

  void ToggleLockOn();

  void SwitchLockTarget_Left();
  void SwitchLockTarget_Right();

private:
  void LockTargetInternal();
  void UnlockTargetInternal();

public:

  UPROPERTY()
  TObjectPtr<AActor> PlayerActor;

  UPROPERTY()
  TObjectPtr<UARHealthComponent> HealthComponent;

  UPROPERTY()
  TObjectPtr<ULockOnComponent> LockOnComponent;

  UPROPERTY()
  TObjectPtr<UCameraRouterComponent> CameraRouter;

  FVector ChargeStartFaceDir; 

  FVector ClimbSurfaceNormal;
  
  FVector SnapTargetImpactPoint;
  
  FVector SnapStartPosition;
  
  FRotator SnapStartRotation;

  FVector2D TargetSnapInputDirection; 
  
  UPROPERTY(EditAnywhere, Category = "PlayerModel|Knockback")
  double LaunchPower;

  UPROPERTY(EditAnywhere, Category = "PlayerModel|Charge")
  double ChargeRotateHalfRange;

  UPROPERTY(EditAnywhere, Category = "PlayerModel|TargetSnap")
  double TargetSnapDetectMaxRange;

  UPROPERTY(EditAnywhere, Category = "PlayerModel|TargetSnap")
  float SnapTimeInterval;

  float SnapTimeCounter;

  TWeakObjectPtr<AActor> SnapTargetActor;

  TWeakObjectPtr<AActor> LockOnTarget;

  FOnCameraRigChanged RigChangeEvent;

  FOnLockOnTargetUpdated LockOnTargetUpdateEvent;

  ECameraRigType CurrentRigType;

  uint8 bIsCharging : 1;
  uint8 bIsInAir : 1;
  uint8 bIsClimbing : 1;
  uint8 bIsInComboAction : 1;
  uint8 bCanUpdateSnapMovement : 1;
  uint8 bIsReadyToSearchSnapTarget : 1;
  uint8 bCanLockOn : 1;
  uint8 bIsLockingOn : 1;

private:
  ESwitchTargetState m_switchLockTargetState;

private:
  AActor* GetTopPriorityTarget(const TArray<AActor*>& InCandidates) const;
  
  bool IsTargetValidToLockOn(const AActor* InTarget) const;

  void ConsumeSwitchTargetState();

};

UCLASS(Blueprintable, BlueprintType)
class UARPlayerPresenter : public UObject,
                           public IARPhysicsSystemHost
{
	GENERATED_BODY()

public:

  /**
   * @brief プレゼンターを初期化する
   * @param InViewCharacter プレイヤーキャラクター
   */
  UE_API void Initialize(AARRangerCharacter* InViewCharacter, APlayerController* InPlayerController);

  /**
   * @brief プレゼンターを解放する
   */
  UE_API void Deinitialize();

  /**
   * @brief 左スティック入力を処理する
   * @param InX X方向入力
   * @param InY Y方向入力
   * @param InDeadZone 入力デッドゾーン(Scalar)
   * @param InMinInput デッドゾーンを超えた入力の最小値（Scalar）
   */
  UE_API void Input_HandleLeftStick(double InX, double InY, /**TODO */double InDeadZone, /**TODO */double InMinInput);

  /**
   * @brief 変身入力を処理する
   */
  UE_API void Input_HandleTransform();

  /**
   * @brief カメラリセット入力を処理する
   */
  UE_API void Input_HandleCameraReset();

  UE_API void Input_HandleLockOn();

  UE_API void Input_HandleRightStick(double InX, double InY);

  /**
   * @brief 溜め攻撃開始通知
   */
  UE_API void OnChargeStartHandled(); 

  /**
   * @brief 溜め攻撃終了通知
   */
  UE_API void OnChargeEndHandled();

private:

  /**
   * @brief プレイヤー移動入力を処理する
   * @param InX　　　　　X軸入力値(Scalar, X-Axis)
   * @param InY         Y軸入力値(Scalar, Y-Axis)
   * @param InDeadZone  デッドゾーン(Scalar)
   * @param InMinInput　デッドゾーンを超えた入力の最小値（Scalar）
   */
  void HandleCharacterMove(double InX, double InY, /**TODO */double InDeadZone, /**TODO */double InMinInput);

  /**
   * @brief プレイヤー吸着攻撃ターゲット更新を処理する
   * @param InX　　　　　X軸入力値(Scalar, X-Axis)
   * @param InY         Y軸入力値(Scalar, Y-Axis)
   */
  void HandleSnapTargetUpdate(double InX, double InY);
    
  /**
   * @brief プレイヤーチャージ時の回転処理をする
   * 
   * @param InX　　　　　X軸入力値(Scalar, X-Axis)
   * @param InY         Y軸入力値(Scalar, Y-Axis)
   */
  void HandleCharacterChargeRotate(double InX, double InY);

  /**
   * @brief 吸着処理タスク(Tick)を登録する
   * @see UpdateSnapTarget
   */
  void RegisterSnapTargetTask();

  /**
   * @brief ターゲットに吸着する処理(Tick)
   * @param DeltaTime 
   */
  void UpdateSnapTarget(float DeltaTime);

  /**
   * @brief 吸着処理タスク(Tick)を解読する
   */
  void UnregisterSnapTargetTask();

  /**
   * @brief 戦闘結果を処理する
   * 
   * @param InAffectedCharacter プレイヤーキャラクター 
   * @param InDamageResult      戦闘ダメージ結果構造体
   */
  void HandleBattleResult(AARRangerCharacter* InAffectedCharacter, const ARRanger::Battle::FARDamageResult& InDamageResult);

  /**
   * @brief バトルステート変更を処理する
   * @param bIsInBattle 
   */
  void HandleBattleStateChange(bool bIsInBattle);

  /**
   * @brief 変身イベントを処理する
   * 
   * @param InNewTransformation 新しい変身状態
   */
  void HandleTransformedEvent(EARMagnetismType InNewTransformation);

  /**
   * @brief プレイヤー攻撃開始時の処理をする
   * 
   */
  void HandleAttackAbilityStarted();

  /**
   * @brief プレイヤー攻撃終了時の処理をする
   */
  void HandleAttackAbilityEnded();

  /**
   * @brief プレイヤーが登るのを始める処理
   */
  void StartClimbing();

  /**
   * @brief プレイヤーが登るのを止める処理
   */
  void StopClimbing();

  /**
   * @brief プレイヤーが登っているときの更新処理
   * @param DeltaTime 
   */
  void UpdateClimbing(float DeltaTime);

  /**
   * @brief 吸着ターゲットの吸着先の座標を更新する
   * 
   * @param InOverlappedComp 探知した吸着ターゲットコンポーネント
   * @param InImpactPoint    当たり判定で当たった座標
   */
  void UpdateSnapTargetImpactPoint(const USceneComponent* InOverlappedComp, const FVector InImpactPoint);

  /**
   * @brief プレイヤーが着地した時呼び出されるコールバック
   */
  UFUNCTION()
  void OnGroundLanded(const FHitResult& InHit);

  UFUNCTION()
  void OnClimbSurfaceOverlapBegan(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
  
  UFUNCTION()
  void OnClimbSurfaceOverlapEnded(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

  UFUNCTION()
  void OnMagneticForceFieldBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

  UFUNCTION()
  void OnMagneticForceFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

  UFUNCTION()
  void OnMagnetizedObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

  void OnCharacterJumpStarted();
  void OnCharacterJumpStopped();
  bool CanUpdateClimbingInternal() const;
  void StopSnapTargetInternal();

  void OnCameraRigChanged(ECameraRigType OldType, ECameraRigType NewType);

  void OnLockOnDataUpdated(const FVector& CameraPos, const FRotator& CameraRot, const TArray<AActor*>& Targets);
  void OnLockOnTargetUpdated(AActor* TargetActor);
  void FilterTargetsInCamera(const FVector& CameraPos, const FRotator& CameraRot, const TArray<AActor*>& OriginTargets, TArray<AActor*>& OutTargets);
  bool IsActorInCameraView(AActor* TargetActor) const;
  bool IsActorInFrontOfPlayer(const FVector& CameraPos, AActor* TargetActor) const;

private:
  /**プレイヤーキャラクター */
  UPROPERTY()
  TObjectPtr<AARRangerCharacter> ViewCharacter;
	
  /**プレイヤーモデル */
  UPROPERTY(EditAnywhere)
  FARPlayerModel Model;

  TWeakObjectPtr<class APlayerController> m_controller;

  /**登る処理デリゲートハンドル */
  FDelegateHandle Handle_UpdateClimbing{};

  /**吸着処理デリゲートハンドル */
  FDelegateHandle Handle_UpdateSnapTarget{};

};

#undef UE_API