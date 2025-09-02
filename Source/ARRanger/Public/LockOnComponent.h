#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy.h"

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
    bool IsTargetVisible(AActor* Target);

    // ターゲット切り替え(右)
    UFUNCTION(BlueprintCallable)
    void SwitchTargetRight();

    // ターゲット切り替え(左)
    UFUNCTION(BlueprintCallable)
    void SwitchTargetLeft();

    // ターゲットを取得
    UFUNCTION(BlueprintPure)
    AActor* GetLockedOnTarget() const { return lockedOnTarget; }

    // ロックオン中フラグを取得
    bool GetIsLockedOn() { return isLockedOn; }

protected:
    // ロックオン可能距離
    UPROPERTY(EditAnywhere, Category = "LockOn")
    float maxLockOnDistance;

    // ロックオン中かどうか
    bool isLockedOn;

    // ロックオン中のターゲット
    UPROPERTY()
    AEnemy* lockedOnTarget;

    // 敵検索用のタグ
    UPROPERTY(EditAnywhere, Category = "LockOn")
    FName enemyTag;

private:
    // ターゲット切り替え処理の関数
    void SwitchTarget(bool IsRight);

    // ロックオン可能な敵を探す
    AEnemy* FindNearestEnemy(AActor* IgnoreActor = nullptr);

    // プレイヤーのオーナー
    APawn* ownerPawn;

    // プレイヤーのコントローラー
    APlayerController* ownerController;
};