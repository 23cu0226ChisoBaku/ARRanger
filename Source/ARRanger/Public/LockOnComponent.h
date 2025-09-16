#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/Enemy_Zako.h"

#include "LockOnComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARRANGER_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    ULockOnComponent();

    virtual void BeginPlay() override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ロックオン関数
    UFUNCTION(BlueprintCallable)
    void ToggleLockOn();

    // 敵がプレイヤーから見えているか判定
    bool IsTargetVisible(TWeakObjectPtr<AActor> Target);

    // ターゲット切り替え(右)
    UFUNCTION(BlueprintCallable)
    void SwitchTargetRight();

    // ターゲット切り替え(左)
    UFUNCTION(BlueprintCallable)
    void SwitchTargetLeft();

    // ターゲットを取得
    UPROPERTY()
    TWeakObjectPtr<AEnemy_Zako> lockedOnTarget;

    // ロックオン中フラグを取得
    bool GetIsLockedOn() { return isLockedOn; }

    // ロックオン中の敵を取得
    AEnemy_Zako* GetLockedOnTarget() const { return lockedOnTarget.IsValid() ? lockedOnTarget.Get() : nullptr; }

protected:
    // ロックオン可能距離
    UPROPERTY(EditAnywhere, Category = "LockOn")
    float maxLockOnDistance;

    // ロックオン中かどうか
    bool isLockedOn;

    // 敵検索用のタグ
    UPROPERTY(EditAnywhere, Category = "LockOn")
    FName enemyTag;

private:
    // ターゲット切り替え処理の関数
    void SwitchTarget(bool IsRight);

    // ロックオン可能な敵を探す
    AEnemy_Zako* FindNearestEnemy(TWeakObjectPtr<AEnemy_Zako> IgnoreActor = nullptr);

    // プレイヤーのオーナー
    UPROPERTY()
    APawn* ownerPawn;

    // プレイヤーのコントローラー
    UPROPERTY()
    APlayerController* ownerController;

public:
    // ロックオンフラグをセット
    void SetIsLockedOn(bool IsLockedOn) { isLockedOn = IsLockedOn; }
};