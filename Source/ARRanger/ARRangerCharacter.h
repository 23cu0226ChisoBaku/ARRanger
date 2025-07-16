#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"

#include "ARRangerCharacter.generated.h"

class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;

struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  シンプルでプレイヤーが操作可能な三人称視点キャラクター
 *  制御可能な軌道カメラの実装
 */
UCLASS(abstract)
class AARRangerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** カメラをキャラクターの背後に配置するカメラブーム */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** フォローカメラ */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:
	virtual void BeginPlay() override;

	// ジャンプアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* JumpAction;

	// 移動アクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* MoveAction;

	// 視点回転アクション(ゲームパッド)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* LookAction;

	// 視点回転アクション(マウス)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
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

	// パンチアニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* PunchMontage;

	// キックアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* KickAction;

	// キックアニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* KickMontage;

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
	bool bIsLockedOn;

	// ロックオン時敵切り替えの可能フラグ
	bool isAbleToSwitchTarget;

	// もともとのカメラとプレイヤーの距離
	float DefaultArmLength;

	// ダッシュ中に近づける距離
	float DashArmLength; 

	// 補間速度
	float ArmLengthInterpSpeed; 

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
	void Punch();

	// キックの際に呼び出される
	void Kick();

public:

	// コントロールまたはUIインターフェースからの移動入力を処理する
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	// コントロールまたはUIインターフェースからのルック入力を処理する
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	// コントロールまたはUIインターフェースのどちらからでも、押されたジャンプ入力を処理する
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	// コントロールまたはUIインターフェースのどちらからでも、押されたジャンプ入力を処理する
	UFUNCTION(BlueprintCallable, Category="Input")
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

	// ロックオン対象
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

	// ロックオン可能距離
	UPROPERTY(EditAnywhere, Category = "LockOn")
	float maxLockOnDistance;

	// ダッシュ中フラグ
	UPROPERTY(BlueprintReadWrite)
	bool isDashed;

	// 移動入力の閾値(これを超えるとダッシュに遷移する)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float moveThreshold;

	// 攻撃力
	int attackPower;

	// パンチの当たり判定用
	UPROPERTY(EditAnywhere, Category = "Combat")
	float punchRadius;

	// キックの当たり判定用
	UPROPERTY(EditAnywhere, Category = "Combat")
	float kickRadius;

	// 攻撃中フラグ
	UPROPERTY(BlueprintReadOnly)
	bool isAttacked;

public:
	virtual void Tick(float DeltaTime) override;

	// CameraBoomサブオブジェクトを返す
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCameraサブオブジェクトを返す
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};