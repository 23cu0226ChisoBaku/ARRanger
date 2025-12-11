#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "ARRangerAnimInstance.generated.h"

#define UE_API ARRANGER_API

UCLASS()
class UARRangerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
  UE_API virtual void NativeUpdateAnimation(float DeltaSeconds) override;
  UE_API virtual void NativePostEvaluateAnimation() override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    float ClimbUpSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    float ClimbRightSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb")
    bool IsClimbing = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
    bool bIsBattled = false;

    // 落下中フラグ
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall")
    bool IsFalled = false;

    // 落下中の時間を保存
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fall")
    float InFallingTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ARRanger|Anim")
    float AnimModifiedSpeed = 1.0f;
};

#undef UE_API