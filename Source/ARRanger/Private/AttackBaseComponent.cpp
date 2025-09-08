#include "AttackBaseComponent.h"

#include "ARRangerCharacter.h"
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

void UAttackBaseComponent::PlayAttackMontage(const FAttackData& Attack)
{
    // �v���C���[�����Ȃ����AMontage���ݒ肳��Ă��Ȃ���Ώ������Ȃ�
    AARRangerCharacter* Char = Cast<AARRangerCharacter>(ownerPawn);
    if (!Char || !Attack.Montage_Normal)
    {
        return;
    }

    // AnimInstance���Ȃ���Ώ������Ȃ�
    UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
    if (!Anim)
    {
        return;
    }

    // ���ɓ��������^�[�W���Đ����Ȃ炱���ŉ������Ȃ�(�p���`��StartPunch���ʓ|������)
    if (Anim->Montage_IsPlaying(Attack.Montage_Normal))
    {
        return;
    }

    bIsAttacked = true;
    Char->SetIsAttacked(true);

    if (bIsStrongAttacked && Attack.Montage_Strong)
    {
        Anim->Montage_Play(Attack.Montage_Strong);
    }
    else
    {
        Anim->Montage_Play(Attack.Montage_Normal);
    }
}

void UAttackBaseComponent::AttackHit(const FAttackData& Attack)
{
  AActor* avatarActor = GetOwner();
  if (avatarActor == nullptr)
  {
    return;
  }

  const float AttackOffset = 100.0f;
  const FVector Origin = avatarActor->GetActorLocation() + avatarActor->GetActorForwardVector() * AttackOffset;
  TArray<AActor*> HitActors{};

  const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
                      avatarActor,
                      Origin,
                      Attack.HitRadius,
                      { UEngineTypes::ConvertToObjectType(ECC_Pawn), UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
                      nullptr,
                      { avatarActor },
                      HitActors
                    );
  if (!bHit)
  {
    return;
  }

  for (AActor* HitActor : HitActors)
  {    

    // Edited by MAI
    static UClass* attackableInterfaceClass = UARAttackable::StaticClass();
    IARAttackable* attackable{nullptr};

    // Check if Actor implements interface
    if (HitActor->GetClass()->ImplementsInterface(attackableInterfaceClass))
    {
      attackable = ::Cast<IARAttackable>(HitActor);
    }

    // Search if any component in actor implements interface
    if (attackable == nullptr)
    {
      TArray<UActorComponent*> interfaceImplementedComps = HitActor->GetComponentsByInterface(attackableInterfaceClass);
      // Mark an exception if more than 1 component implements IARAttackable
      check(interfaceImplementedComps.Num() < 2);
      if (interfaceImplementedComps.Num() > 0)
      {
        attackable = ::Cast<IARAttackable>(interfaceImplementedComps[0]);
      }
    }

    // Could not find IARAttackable, Stop attacking
    if (attackable == nullptr)
    {
      UE_LOG(LogTemp, Error, TEXT("Actor or ActorComponents are not implemented [%s]. Actor name: [%s]"), *attackableInterfaceClass->GetName(), *avatarActor->GetName());
      return;
    }

    /**
     * @brief 実際の攻撃処理。処理を別の場所に移動したい場合はヌルチェックを含めて全て移動するようにお願いします。
     * @see   #include "BattleSystem/IARAttackable.h"
     * @see   #include "BattleSystem/IARAttackerInterface.h"
     */
    {
      // 攻撃ダメージ計算や飛ばす計算処理
      FARAttackParameters attackParam{};
      attackParam.Instigator = avatarActor;
      attackParam.bUseAttackerActor = false;
      
      attackParam.Damage = Attack.Damage;
      if (bIsStrongAttacked)
      {
        attackParam.Damage += Attack.DamageModifier;
      }

      FVector LaunchDir{EForceInit::ForceInitToZero};
      if (bIsBlowedAwayEnemy)
      {
        const FVector ImpulseDir_Norm = ((HitActor->GetActorLocation() - avatarActor->GetActorLocation()).GetSafeNormal() + FVector{0.0, 0.0, 0.5}).GetSafeNormal();
        LaunchDir = avatarActor->GetActorForwardVector() + FVector{0.0, 0.0, 0.2};
        LaunchDir.Normalize();
        LaunchDir += ImpulseDir_Norm;
        LaunchDir.Normalize();
      }
      attackParam.LaunchDirection = LaunchDir;

      // ターゲットを攻撃する
      (void)attackable->AttackTarget(::Cast<IARAttackerInterface>(avatarActor), attackParam);

      // Hitエフェクトを生成
      const FVector SpawnLocation = HitActor->GetActorLocation();
      const FRotator SpawnRotation = FRotator::ZeroRotator;
      GetWorld()->SpawnActor<AActor>(Attack.HitEffectActor, SpawnLocation, SpawnRotation);   
    }
  }
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