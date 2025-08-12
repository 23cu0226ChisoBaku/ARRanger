#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "ARRangerAnimInstance.generated.h"

UCLASS()
class ARRANGER_API UARRangerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
    // 引力クライム時の上下方向の速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    float ClimbUpSpeed;

    // 引力クライム時の左右方向の速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    float ClimbRightSpeed;

    // 引力クライム中フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    bool IsClimbing = false;

    // 歩き状態遷移フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool ShouldMove = false;

    // 歩き状態終了フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool FinishWalk = false;

    // 歩き状態終了時に呼ばれる
    UFUNCTION(BlueprintCallable)
    void RequestFinishWalk();

private:
    // 歩き状態終了後のの時間を計測
    float FinishStartTime = 0.f;

    // 半歩分のアニメーションの時間
    float FinishDuration = 0.3f;
};