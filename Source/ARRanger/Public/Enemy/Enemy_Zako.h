#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleSystem/IARAttackable.h" 
#include "Enemy_Zako.generated.h"

UCLASS()
class ARRANGER_API AEnemy_Zako : public ACharacter, public IARAttackable
{
    GENERATED_BODY()

public:
    AEnemy_Zako();

    void SetIsChasing(bool bChasing);

    // 既存のダメージ処理
    void ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    int32 maxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    int32 currentHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    bool isDead;

    // 引き寄せ開始関数
    void StartAttraction(AActor* Target);

    // IARAttackable のオーバーライド

private:
    // 引き寄せられ中フラグ
    bool bIsAttracted = false;

    // 引き寄せられる対象(プレイヤー)
    AActor* attractionTarget = nullptr;

    // 引き寄せ停止関数
    void StopAttraction();

protected:
    // 引き寄せ時のスピード
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float attractionSpeed = 800.f;

    // 引き寄せの最低距離(これ以下になったらパンチ開始)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    float MinDistance = 150.f;

    virtual void OnPreAttacked(
        const FARAttackParameters& InAttackParams,
        ARRanger::Battle::FARAttackResult& OutAttackResult) override;

    virtual void OnDamaged(
        const ARRanger::Battle::FARDamageResult& InDamageResult) override;

    virtual void OnPostAttacked(
        const FARAttackParameters& InAttackParams) override;
};
