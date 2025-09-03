#pragma once

#include "CoreMinimal.h"
#include "AttackData.generated.h"

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	// 実際に再生する通常攻撃アニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage_Normal;

	// 強攻撃アニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage_Strong;

	// 強攻撃時の引力・斥力のアニメーションモンタージュ(引き寄せ、吹き飛ばし)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage_AR;

	// ヒット時のエフェクトを保存
	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<AActor> HitEffectActor;

	// 当たり判定の半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitRadius;

	// ダメージ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage;

	// 強攻撃用のダメージ補正
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageModifier;

	// 攻撃対象のタグ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetTag; 
};