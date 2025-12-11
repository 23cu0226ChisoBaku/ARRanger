#include "AttackBaseComponent.h"

#include "Character/ARRangerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

#include "BattleSystem/IARAttackable.h"
#include "BattleSystem/IARAttackerInterface.h"

UAttackBaseComponent::UAttackBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

    // �v���C���[�Ƃ��̃R���g���[���[���擾
    ownerPawn = Cast<AARRangerCharacter>(GetOwner());
    ownerController = ownerPawn ? Cast<APlayerController>(ownerPawn->GetController()) : nullptr;
}

void UAttackBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttackBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttackBaseComponent::RotateOwnerToTarget()
{
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(ownerPawn);
    if (!Char || !Char->LockOnComponent || !Char->LockOnComponent->GetIsLockedOn())
    {
        return;
    }

    AActor* Target = Char->LockOnComponent->GetLockedOnTarget();
    if (!Target)
    {
        return;
    }
        
    FVector Dir = Target->GetActorLocation() - Char->GetActorLocation();
    Dir.Z = 0.f;
    Dir.Normalize();

    FRotator TargetRot = Dir.Rotation();
    Char->SetActorRotation(TargetRot);
}

void UAttackBaseComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogTemp, Warning, TEXT("Montage End"));
    bIsAttacked = false;
    bIsStrongAttacked = false;
    bIsBlowedAwayEnemy = false;
    bIsAttractingEnemy = false;

    if (AARRangerCharacter* Char = Cast<AARRangerCharacter>(ownerPawn))
    {
        Char->SetIsAttacked(false);
        Char->SetIsStrongAttacked(false);
        Char->ResetComboCount();
    }
}