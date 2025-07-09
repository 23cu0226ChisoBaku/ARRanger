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

	// パンチアクション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PunchAction;

	// パンチアニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* PunchMontage;

public:

	/** コンストラクタ */
	AARRangerCharacter();	

protected:

	/** 入力アクションのバインディングを初期化する */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	/** 移動入力のために呼び出される */
	void Move(const FInputActionValue& Value);

	/** 入力を求める */
	void Look(const FInputActionValue& Value);

private:
	// ロックオン中フラグ
	bool bIsLockedOn;

	// 現在のコンボ数
	int32 CurrentCombo = 0;

	// 次のコンボ攻撃を行えるかのフラグ
	bool bCanNextCombo = false;

	// 攻撃しているかのフラグ
	bool bIsAttacking = false;

	// ロックオン切替関数
	void ToggleLockOn();

	// スティック入力で敵切替（右スティックのX軸など）
	void SwitchTarget(float Value);

	// ロックオン可能な敵を検索
	AActor* FindNearestEnemy();

	// パンチボタンが押されたときに呼び出される
	void OnAttackPressed();
	
	// コンボアニメーションを再生
	void PlayComboMontage(int32 ComboIndex);

	// コンボができる状態にする
	void EnableCombo();

	// 攻撃のヒット判定
	void AttackHitCheck();

	// コンボが終了した際に呼び出される
	void ComboEnd();

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

	// ロックオン対象
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* LockedOnTarget;

public:
	virtual void Tick(float DeltaTime) override;

	// CameraBoomサブオブジェクトを返す
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// FollowCameraサブオブジェクトを返す
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};