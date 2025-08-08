#pragma once

#include "AttackData.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AttackComponent.generated.h"

class AARRangerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARRANGER_API UAttackComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAttackComponent();

    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // パンチの際に呼び出される
    void StartPunch();

    // キックの際に呼び出される
    void Kick();

    // パンチのAnimNotifyの通知を受け取る
    UFUNCTION(BlueprintCallable)
    void PunchHitNotify();

    // キックのAnimNotifyの通知を受け取る
    UFUNCTION(BlueprintCallable)
    void KickHitNotify();

    // 攻撃が終わった際のコールバック
    UFUNCTION()
    void OnAttackMontageEnded(UAnimMontage* Montage, bool IsInterrupted);

private:
    // 敵を引き寄せ中のフラグ
    bool isAttractingEnemy;

    // 強い攻撃かどうかのフラグ
    bool isStrongAttack;

    // 攻撃アニメーションの再生用関数
    void PlayAttackMontage(const FAttackData& Attack);

    // 当たり判定の処理
    void AttackHit(const FAttackData& Attack);

    // プレイヤーのオーナー
    AARRangerCharacter* ownerPawn;

    // プレイヤーのコントローラー
    APlayerController* ownerController;

public:
    // パンチデータ（Blueprintから設定）
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData PunchData;

    // キックデータ（Blueprintから設定）
    UPROPERTY(EditAnywhere, Category = "Attack")
    FAttackData KickData;

    // 攻撃中フラグ
    UPROPERTY(BlueprintReadOnly)
    bool IsAttacked;

    // ヒットエフェクト用のアクター
    UPROPERTY(EditAnywhere, Category = "Effects")
    TSubclassOf<AActor> HitEffectActor;

    // 引き寄せ中フラグを取得
    bool GetIsAttractingEnemy() { return isAttractingEnemy; }

    // 攻撃中フラグを取得
    bool GetIsAttacked() { return IsAttacked; }

    //強攻撃フラグを取得
    bool GetIsStrongAttacked() { return isStrongAttack; }
};