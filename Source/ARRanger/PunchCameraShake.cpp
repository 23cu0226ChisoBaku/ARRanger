#include "PunchCameraShake.h"

UPunchCameraShake::UPunchCameraShake(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ElapsedTime = 0.f;
    Duration = 0.4f; // �h��̎���
}

void UPunchCameraShake::StartShake(
    APlayerCameraManager* InCameraManager,
    float Scale,
    ECameraShakePlaySpace InPlaySpace,
    FRotator UserPlaySpaceRot)
{
    Super::StartShake(InCameraManager, Scale, InPlaySpace, UserPlaySpaceRot);
    ElapsedTime = 0.f;
}

bool UPunchCameraShake::IsFinished() const
{
    return ElapsedTime >= Duration;
}

void UPunchCameraShake::UpdateAndApplyCameraShake(
    float DeltaTime,
    float Alpha,
    FMinimalViewInfo& InOutPOV)
{
    ElapsedTime += DeltaTime;

    float ShakeStrength = FMath::Lerp(1.0f, 0.0f, ElapsedTime / Duration);

    // �����_���ɃJ��������]������
    FRotator RandomRot;
    RandomRot.Pitch = FMath::FRandRange(-25.0f, 25.0f);
    RandomRot.Yaw = FMath::FRandRange(-20.0f, 20.0f);
    RandomRot.Roll = FMath::FRandRange(-15.0f, 15.0f);

    InOutPOV.Rotation += RandomRot;

    UE_LOG(LogTemp, Warning, TEXT("Applied Shake: %s"), *InOutPOV.Rotation.ToCompactString());
}