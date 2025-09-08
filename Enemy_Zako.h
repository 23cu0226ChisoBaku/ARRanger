#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleSystem/IARAttackable.h" 
#include "ISpecialAttractInterface.h"
#include "Enemy_Zako.generated.h"

UCLASS()
class ARRANGER_API AEnemy_Zako : public ACharacter, public IARAttackable , public ISpecialAttractInterface
{
    GENERATED_BODY()

public:
    AEnemy_Zako();

    void SetIsChasing(bool bChasing);

    // �����̃_���[�W����
    void ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Stats")
    int32 maxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    int32 currentHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy|Stats")
    bool isDead;

    // IARAttackable �̃I�[�o�[���C�h
protected:
    virtual void OnPreAttacked(
        const FARAttackParameters& InAttackParams,
        ARRanger::Battle::FARAttackResult& OutAttackResult) override;

    virtual void OnDamaged(
        const ARRanger::Battle::FARDamageResult& InDamageResult) override;

    virtual void OnPostAttacked(
        const FARAttackParameters& InAttackParams) override;

/**********ISpecialAttractInterface functions Start**********/
    /** 
	 * @brief 引力必殺技が始まった時の通知 
	 */
	void OnStartSpecialAttractNotify() override;

	/**
	 * @brief 引力必殺技の中間通知 
	 * 
	 * @param 経過時間
	 */
	void OnUpdateSpecialAttractNotify(float elapsed) override;

	/**
	 * @brief 引力必殺技の終了通知
	 */
	void OnEndSpecialAttractNotify() override;

/**********ISpecialAttractInterface functions End**********/
};
