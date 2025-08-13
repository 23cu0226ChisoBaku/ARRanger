#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "ARRangerAnimInstance.generated.h"

UCLASS()
class ARRANGER_API UARRangerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

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

    // 速度（cm/s）
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    float Speed = 0.f;

    // アニメを最低限継続させるフラグ
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
    bool bForceMoveAnim = false;
};