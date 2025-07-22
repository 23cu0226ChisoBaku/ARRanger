#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "PunchCameraShake.generated.h"

UCLASS()
class ARRANGER_API UPunchCameraShake : public UCameraShakeBase
{
    GENERATED_BODY()

public:
    UPunchCameraShake(const FObjectInitializer& ObjectInitializer);

    void StartShake(
        class APlayerCameraManager* InCameraManager,
        float Scale,
        ECameraShakePlaySpace InPlaySpace,
        FRotator UserPlaySpaceRot);

    bool IsFinished() const;

    void UpdateAndApplyCameraShake(
        float DeltaTime,
        float Alpha,
        FMinimalViewInfo& InOutPOV);

private:
    float ElapsedTime;
    float Duration;
};