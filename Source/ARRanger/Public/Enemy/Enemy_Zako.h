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

    // IARAttackable のオーバーライド
protected:
    virtual void OnPreAttacked(
        const FARAttackParameters& InAttackParams,
        ARRanger::Battle::FARAttackResult& OutAttackResult) override;

    virtual void OnDamaged(
        const ARRanger::Battle::FARDamageResult& InDamageResult) override;

    virtual void OnPostAttacked(
        const FARAttackParameters& InAttackParams) override;
};
