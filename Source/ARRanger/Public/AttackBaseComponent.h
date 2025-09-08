#pragma once

#include "AttackData.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AttackBaseComponent.generated.h"

class AARRangerCharacter;
class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API UAttackBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackBaseComponent();

protected:
	virtual void BeginPlay() override;

	// AbilitySystemComponentを保存
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComp;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// プレイヤーを保存
	AARRangerCharacter* ownerPawn = nullptr;

	// プレイヤーのコントローラーも保存
	APlayerController* ownerController = nullptr;

	// 攻撃中フラグ
	bool bIsAttacked = false;

	// 強攻撃フラグ
	bool bIsStrongAttacked = false;

	// 敵吹き飛ばしフラグ
	bool bIsBlowedAwayEnemy = false;

	// 敵引き寄せフラグ
	bool bIsAttractingEnemy = false;

public:
	// 引き寄せ中フラグを取得
	bool GetIsAttractingEnemy() { return bIsAttractingEnemy; }

	// 引き寄せ中フラグをセット
	void SetIsAttractingEnemy(bool IsAttracting) { bIsAttractingEnemy = IsAttracting; }

	// 敵吹き飛ばしフラグをセット
	void SetIsBlowedAwayEnemy(bool IsBlowedAwayEnemy) { bIsBlowedAwayEnemy = IsBlowedAwayEnemy; }

	// 攻撃中フラグを取得
	bool GetIsAttacked() { return bIsAttacked; }

	// 攻撃中フラグをセット
	void SetIsAttacked(bool IsAttacked) { bIsAttacked = IsAttacked; }

	// 強攻撃フラグを取得
	bool GetIsStrongAttacked() { return bIsStrongAttacked; }

	// 強攻撃中フラグをセット
	void SetIsStrongAttacked(bool IsStrongAttacked) { bIsStrongAttacked = IsStrongAttacked; }

	//　プレイヤーを敵の方向に向かせる
	void RotateOwnerToTarget();

	// 攻撃モンタージュを再生
	void PlayAttackMontage(const FAttackData& Attack);

	// 攻撃ヒット判定用関数
	void AttackHit(const FAttackData& Attack);

	// Montage終了イベント
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};