#pragma once

#include "AttackData.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InsekiClimbingObject.h"
#include "Logging/LogMacros.h"
#include "PlayerObservation/IObservableSubjectInterface.h"
#include "Public/IARMagnetizableInterface.h"

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
UCLASS(abstract)
class AARRangerCharacter : public ACharacter,
                           public IObservableSubjectInterface
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
	// ロックオン中フラグ
	bool isLockedOn;

	// ロックオン時敵切り替えの可能フラグ
	bool isAbleToSwitchTarget;

	// もともとのカメラとプレイヤーの距離
	float DefaultArmLength;

	// ダッシュ中に近づける距離
	float DashArmLength;

	// 補間速度
	float ArmLengthInterpSpeed;

	// 敵がプレイヤーから見えているか判定
	bool IsTargetVisible(AActor* Target);

	// ロックオン切替関数
	void ToggleLockOn();

	// 十字ボタン右を押した際に呼び出される
	void SwitchTargetRight();

	// 十字ボタン左を押した際に呼び出される
	void SwitchTargetLeft();

	// ロックオン時ターゲット切り替え関数(引数によって前後に切り替え)
	void SwitchTarget(bool isPressedRight);

	// ロックオン可能な敵を検索
	AActor* FindNearestEnemy(AActor* IgnoreActor = nullptr);

	// パンチの際に呼び出される
	void StartPunch();

	// キックの際に呼び出される
	void Kick();

	// 攻撃アニメーションの再生用関数
	void PlayAttackMontage(const FAttackData& Attack);

	// 当たり判定の処理
	void AttackHit(const FAttackData& Attack);

	// 変身の際に呼び出される
	void Transform();

	// ダッシュ時カメラが切り替わる入力の閾値（押し込み時）
	float dashStartThreshold;

	// 少し入力を緩めたらダッシュを解除する用の数値
	float dashEndThreshold;

	// 敵を引き寄せ中のフラグ
	bool isAttractingEnemy;

	// 強い攻撃かどうかのフラグ
	bool isStrongAttack;

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

	// パンチのAnimNotifyの通知を受け取る
	UFUNCTION(BlueprintCallable)
	void PunchHitNotify();

	// キックのAnimNotifyの通知を受け取る
	UFUNCTION(BlueprintCallable)
	void KickHitNotify();

	// 攻撃が終わった際のコールバック
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool IsInterrupted);

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

	// ロックオン対象
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

	// ロックオン可能距離
	UPROPERTY(EditAnywhere, Category = "LockOn")
	float maxLockOnDistance;

	// ダッシュ中フラグ
	UPROPERTY(BlueprintReadWrite)
	bool isDashed;

	// パンチデータ（Blueprintから設定）
	UPROPERTY(EditAnywhere, Category = "Attack")
	FAttackData PunchData;

	// キックデータ（Blueprintから設定）
	UPROPERTY(EditAnywhere, Category = "Attack")
	FAttackData KickData;

	// 攻撃中フラグ
	UPROPERTY(BlueprintReadOnly)
	bool isAttacked;

	// 現在のプレイヤーの変身状態
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	EARMagnetismType CurrentARType;

	// 引力クライム時のアニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	UAnimMontage* Montage_AttractionClimb;

public:
	virtual void Tick(float DeltaTime) override;

	// 現在のプレイヤーのモードを取得
	UFUNCTION(BlueprintCallable)
	EARMagnetismType GetCurrentARType();

	// 麦
	bool bIsJumping = false;

	UFUNCTION()
	void LandedToGround(const FHitResult& Hit)
	{
		bIsJumping = false;
	}
};