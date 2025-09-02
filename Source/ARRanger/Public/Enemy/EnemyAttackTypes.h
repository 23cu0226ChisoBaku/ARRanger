#pragma once

#include "CoreMinimal.h"
#include "EnemyAttackTypes.generated.h"

/*
 * 敵の攻撃タイプを列挙
 * Blueprint でも使えるように UENUM(BlueprintType) を指定
 */
UENUM(BlueprintType)
enum class EAttackType : uint8
{
    None        UMETA(DisplayName = "None"),          // 攻撃なし
    Punch       UMETA(DisplayName = "Punch"),         // パンチ攻撃
    Kick        UMETA(DisplayName = "Kick"),          // キック攻撃
    JumpAttack  UMETA(DisplayName = "Jump Attack"),   // ジャンプ攻撃
    Roar        UMETA(DisplayName = "Roar")           // 吠え攻撃（演出用）
};
