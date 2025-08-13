#pragma once

#include "AttackComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IARMagnetizableInterface.h"
#include "InsekiClimbingObject.h"
#include "LockOnComponent.h"
#include "Logging/LogMacros.h"
#include "Physics/IARPhysicsSystemHost.h"
#include "PlayerObservation/IObservableSubjectInterface.h"

#include "ARRangerCharacter.generated.h"

class UAnimMontage;
class USkeletalMesh;
class UInputAction;

struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  シンプルでプレイヤーが操作可能な三人称視点キャラクター
 *  制御可能な軌道カメラの実装
 */
UCLASS(Abstract)
class AARRangerCharacter :  public ACharacter,
                            public IObservableSubjectInterface,
                            public IARMagnetizableInterface,
                            public IARPhysicsSystemHost
{
	GENERATED_BODY()
	
	protected:
	virtual void BeginPlay() override;

	// 麦
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ジャンプアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	// 移動アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	// 山内　引力付与アクション 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttachAttractionAction;

	// 山内　斥力付与アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttachRepulsionAction;

	// 視点回転アクション(ゲームパッド)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	// 視点回転アクション(マウス)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MouseLookAction;

	// ロックオンアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LockOnAction;

	// ロックオン時ターゲット切り替えアクション(次のターゲット)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchTargetRightAction;

	// ロックオン時ターゲット切り替えアクション(前のターゲット)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SwitchTargetLeftAction;

	// パンチアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PunchAction;

	// キックアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* KickAction;

	// 変身アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* TransformAction;

	// 引力クライムフラグ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool isClimbed;

public:

	// コンストラクタ
	AARRangerCharacter();

protected:

	// 入力アクションのバインディングを初期化する
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// 移動入力のために呼び出される
	void Move(const FInputActionValue& Value);

	// 入力を求める
	void Look(const FInputActionValue& Value);

private:
	// もともとのカメラとプレイヤーの距離
	float DefaultArmLength;

	// ダッシュ中に近づける距離
	float DashArmLength;

	// 補間速度
	float ArmLengthInterpSpeed;

	// 入力の閾値
	const float inputDeadZone = 0.3f;

	// 半歩歩くための時間
	const float minWalkTime = 0.3f;

	// 現在の歩き時間カウンタ
	float walkTimer = 0.0f;

	// 現在の歩き状態フラグ
	bool isWalking = false;

	// 前フレームの歩き状態を記憶
	bool bWasMoving = false; 

	// 移動入力の更新
	void UpdateMovementState();

	// 変身の際に呼び出される
	void Transform();

	// ダッシュ時カメラが切り替わる入力の閾値（押し込み時）
	float dashStartThreshold;

	// 少し入力を緩めたらダッシュを解除する用の数値
	float dashEndThreshold;

	// 現在歩いているオブジェクトの表面
	UPROPERTY()
	AInsekiClimbingObject* currentClimbSurface;

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

	UFUNCTION(BlueprintPure, Category = "AR|Player")
	float GetDefaultArmLength() const { return DefaultArmLength; }

	UFUNCTION(BlueprintPure, Category = "AR|Player")
	float GetDashArmLength() const { return DashArmLength; }

	// 引力用プレイヤーメッシュ
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* AttractionMesh;

	// 斥力用プレイヤーメッシュ
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* RepulsionMesh;

	// ダッシュ中フラグ
	UPROPERTY(BlueprintReadWrite)
	bool IsDashed;

	// ロックオンコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ULockOnComponent* LockOnComponent;

	// アタックコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	UAttackComponent* AttackComponent;

	// 引力クライム時のアニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	UAnimMontage* Montage_AttractionClimb;

public:
	virtual void Tick(float DeltaTime) override;

	// 現在のプレイヤーのモードを取得
	UFUNCTION(BlueprintPure)
	EARMagnetismType GetCurrentARType();

	// 引力クライム中フラグを取得
	bool GetIsClimbed() { return isClimbed; }

	// AttackComponent内で使用するNotifyHandler用
	void OnAttackHitNotify();

	// 麦
	bool bIsJumping = false;

	UFUNCTION()
	void LandedToGround(const FHitResult& Hit)
	{
		bIsJumping = false;
	}

private:
	UFUNCTION()
	void OnMagneticForceFieldBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMagneticForceFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnMagnetizedObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

  /**Start IARMagnetizableInterface interface */
  ARRANGER_API virtual void OnRepulsionEvaluated(const FARMagneticForceResult& Result) override;
	ARRANGER_API virtual AActor* GetActor() override { return this; }
  /**End IARMagnetizableInterface interface */
};