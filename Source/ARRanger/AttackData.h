#pragma once

#include "CoreMinimal.h"
#include "AttackData.generated.h"

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	// 攻撃アニメーションモンタージュ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage = nullptr;

	// 当たり判定の半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitRadius = 100.f;

	// ダメージ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 10;

	// 攻撃対象のタグ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetTag = "Enemy"; 
};