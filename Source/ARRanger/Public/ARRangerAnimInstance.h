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

    // 戦闘中フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
    bool bIsBattled = false;
};