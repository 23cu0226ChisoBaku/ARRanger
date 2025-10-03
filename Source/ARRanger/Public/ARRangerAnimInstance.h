#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "ARRangerAnimInstance.generated.h"

UCLASS()
class ARRANGER_API UARRangerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
    // 引力クライム中フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    bool IsClimbing = false;

    // 戦闘中フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
    bool bIsBattled = false;

    // 落下中フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall")
    bool IsFalled = false;

    // 落下中の時間を保存
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall")
    float InFallingTime = 0.0f;
};