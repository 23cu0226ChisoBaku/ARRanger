#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy_Zako.h"
#include "Enemy_Shooter.generated.h"

UCLASS()
class ARRANGER_API AEnemy_Shooter : public AEnemy_Zako
{
    GENERATED_BODY()

public:
    AEnemy_Shooter();

protected:
    virtual void BeginPlay() override;

    /** ビーム用Projectileクラス */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<AActor> BeamProjectileClass;

    /** ビーム発射ポイント */
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    USceneComponent* MuzzlePoint;

    /** 最適な射程距離 */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float IdealRange;

    /** 許容距離幅 */
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float RangeTolerance;

public:
    /** ビーム発射処理 */
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Zako_PerformAttack()override;
};
