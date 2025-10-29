#include "Character/ARRangerCharacter.h"

#include "Animations/ARRangerAnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerComponents/LockOnComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Player/ARPlayerState.h"
#include "PlayerComponents/AttractSpecialAttackComponent.h"
#include "Sound/SoundBase.h"
#include "Player/CameraRigType.h"

#include "Pawn/ARPawnInitComponent.h"
#include "Character/ARHealthComponent.h"
#include "Character/ARAbilityCostComponent.h"

AARRangerCharacter::AARRangerCharacter()
{
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;
  
  // カプセルコンポーネント初期化
  {
    UCapsuleComponent* capsuleComp = GetCapsuleComponent();
    capsuleComp->InitCapsuleSize(42.f, 96.0f);
  }

  // ムーブコンポーネント初期化
  {
    UCharacterMovementComponent* moveComp = GetCharacterMovement();
    moveComp->bOrientRotationToMovement = true;
    moveComp->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    moveComp->JumpZVelocity = 500.f;
    moveComp->AirControl = 0.35f;
    moveComp->MaxWalkSpeed = 500.f;
    moveComp->MinAnalogWalkSpeed = 20.f;
    moveComp->BrakingDecelerationWalking = 2000.f;
    moveComp->BrakingDecelerationFalling = 1500.0f;
  }

  // ロックオンコンポーネント作成
  LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));

  // HPコンポネント作成
  HealthComponent = CreateDefaultSubobject<UARHealthComponent>(TEXT("HealthComponent"));
  HealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnVignetteEffectChanged);
  HealthComponent->OnDeadEventStarted.AddDynamic(this, &ThisClass::OnPlayerDeadStarted);
  HealthComponent->OnDeadEventFinished.AddDynamic(this, &ThisClass::OnPlayerDeadEnded);

  // アビリティコストコンポネント作成
  AbilityCostComponent = CreateDefaultSubobject<UARAbilityCostComponent>(TEXT("AbilityCostComponent"));
  AbilityCostComponent->OnAbilityCostApplied.AddDynamic(this, &ThisClass::OnAbilityCostHandled);
}

void AARRangerCharacter::BeginPlay()
{
  Super::BeginPlay();

  attractSpecialAttackComponent = FindComponentByClass<UAttractSpecialAttackComponent>();

  // Start with Repulsion
  SetMagnetismType(EARMagnetismType::Repulsion);
  SetCameraRig(ECameraRigType::Default);
  TransformInternal();
}

void AARRangerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* AARRangerCharacter::GetAbilitySystemComponent() const
{
  if (AARPlayerState* playerState = GetPlayerState<AARPlayerState>())
  {
    return playerState->GetAbilitySystemComponent();
  }

  return nullptr;
}

void AARRangerCharacter::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (TickTaskDelegate.IsBound())
  {
    TickTaskDelegate.Broadcast(DeltaTime);
  }

  // bool isLockedOn = LockOnComponent->GetIsLockedOn();
  // AActor* Target = LockOnComponent->GetLockedOnTarget();

  // // ロックオン中に処理
  // if (isLockedOn && Target)
  // {
  //   FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
  //   FRotator TargetRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
  //   TargetRotation.Pitch = 0.f;
  //   TargetRotation.Roll = 0.f;

  //   // ターゲットに向けて回転
  //   SetActorRotation(TargetRotation);

  //   // コントローラーも回転する
  //   if (Controller)
  //   {
  //     FRotator CurrentControlRot = Controller->GetControlRotation();

  //     // 補間も入れて滑らかに回転させる
  //     FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetRotation, DeltaTime, 5.0f);
  //     Controller->SetControlRotation(NewControlRot);
  //   }
  // }

}

void AARRangerCharacter::DoMove(double InRight, double InForward)
{
  if (FMath::IsNearlyZero(InRight) && FMath::IsNearlyZero(InForward))
  {
    return;
  }

  const FRotator Rotation = GetController()->GetControlRotation();
  const FRotator YawRotation{0.0, Rotation.Yaw, 0.0};
  const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
  const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

  AddMovementInput(ForwardDirection, InForward);
  AddMovementInput(RightDirection, InRight);
}

void AARRangerCharacter::DoRotate(double InYaw)
{
  if (FMath::IsNearlyZero(InYaw))
  {
    return;
  }

  const FRotator newCharacterRotator= GetActorRotation() + FRotator{0.0, InYaw, 0.0};
  SetActorRotation(newCharacterRotator);
}

void AARRangerCharacter::OnClimbStarted()
{
  // AnimInstance側のフラグを上げる
  if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    MyAnim->IsClimbing = true;
  }

  GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
  GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AARRangerCharacter::OnClimbUpdated(const FVector& InClimbMovement)
{
  AddActorWorldOffset(InClimbMovement, false);
}

void AARRangerCharacter::OnClimbEnded()
{
  GetCharacterMovement()->SetMovementMode(MOVE_Falling);
  GetCharacterMovement()->bOrientRotationToMovement = true;
  // AnimInstance側のフラグも下げる
  if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    MyAnim->IsClimbing = false;
  }

  // 少し上方向にジャンプさせる
  LaunchCharacter(FVector{0.0, 0.0, 700.0}, true, true);
}

void AARRangerCharacter::OnAttractionCompleted()
{
  // 引き寄せ完了フラグを立てる
  SetIsApproachedEnemy(true);
}

void AARRangerCharacter::ToggleLockOn()
{
  if (LockOnComponent != nullptr)
  {
    LockOnComponent->ToggleLockOn();
  }
}

void AARRangerCharacter::SwitchTargetRight()
{
  if (LockOnComponent != nullptr)
  {
    LockOnComponent->SwitchTargetRight();
  }
}

void AARRangerCharacter::SwitchTargetLeft()
{
  if (LockOnComponent != nullptr)
  {
    LockOnComponent->SwitchTargetLeft();
  }
}

void AARRangerCharacter::OnAttackHitNotify()
{
  // プレイヤー内でのみ扱いたいのでこちらで攻撃のコールバック
  TSharedRef<ARRanger::INotifyHandlerInterface> notifyHandler = GetNotifyHandlerRef();
  notifyHandler->OnAttack();
}

void AARRangerCharacter::Transform()
{
  const EARMagnetismType newMagnetismType = (GetMagnetismType() == EARMagnetismType::Repulsion) ? EARMagnetismType::Attraction : EARMagnetismType::Repulsion;
  SetMagnetismType(newMagnetismType); 
  TransformInternal();
  // Play all transform effects
  PlayTransformEffect();
}

void AARRangerCharacter::PlayTransformEffect()
{
  // 変身エフェクトを再生
  if (TransformEffect != nullptr)
  {
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
      GetWorld(),
      TransformEffect,
      GetActorLocation(),
      GetActorRotation(),
      FVector(1.0f),
      true,
      true,
      ENCPoolMethod::AutoRelease
    );
  }

  // 変身音を再生
  UGameplayStatics::PlaySound2D(
    GetWorld(),
    SE_Transform,
    1.0f,
    1.0f
  );
}

void AARRangerCharacter::TransformInternal()
{
  const EARMagnetismType curtType = GetMagnetismType();
  if (OnTransformed.IsBound())
  {
    OnTransformed.Broadcast(curtType);
  }

  // Blueprint call
  {
    K2_OnTransformed(curtType);
  }
}

bool AARRangerCharacter::CanSpecialAttractAttack()
{

  // To Do：引力フックショット時、必殺技ゲージ非満タン時、被ダメージ時にもfalseを返す

  // 可能ならtrueを返す
  return true;
}

void AARRangerCharacter::OnSpecialAttractAttack()
{
  // 必殺技が使用可能でなければ処理しない
  if (!CanSpecialAttractAttack())
  {
    return;
  }

  // 敵のロックオンを解除しておく
  if (LockOnComponent != nullptr) 
  {
    LockOnComponent->SetIsLockedOn(false);
  }

  // キャラクターの速度をあらかじめ0にしておく
  GetCharacterMovement()->StopMovementImmediately();

  // To Do：必殺技の途中に攻撃を食らわないよう、プレイヤーを無敵状態にしておく

  if (attractSpecialAttackComponent != nullptr)
  {
    attractSpecialAttackComponent->StartSpecialAttract();
  }
}

EARMagnetismType AARRangerCharacter::GetCurrentARType() const
{
  return GetMagnetismType();
}

void AARRangerCharacter::ResetIsAttacked()
{
  // TODO Need implementation
}

void AARRangerCharacter::SetIsBattledInAnimInstance(const bool IsBattled)
{
  if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    MyAnim->bIsBattled = IsBattled;
  }

  if (OnBattleStateChanged.IsBound())
  {
    OnBattleStateChanged.Broadcast(IsBattled);
  }

  // TODO
  if (IsBattled)
  {
    K2_OnBattleStarted();
  }
  else
  {
    K2_OnBattleEnded();
  }
}

void AARRangerCharacter::OnRepulsionEvaluated(const FARMagneticForceResult& Result)
{
  LaunchCharacter(Result.FinalForce, false, false);
}

/**Start IARAttackable implementation */
bool AARRangerCharacter::CanAttack()
{
  return true;
}

void AARRangerCharacter::OnPreAttacked(const FARAttackParameters& InAttackParams, ARRanger::Battle::FARAttackResult& OutAttackResult)
{
  OutAttackResult.Result = ARRanger::Battle::EARAttackResult::Success;
}

void AARRangerCharacter::OnPostAttacked(const FARAttackParameters& InAttackParams)
{

}

void AARRangerCharacter::OnDamaged(const ARRanger::Battle::FARDamageResult& InDamageResult)
{
  if (OnBattleResultAccepted.IsBound())
  {
    OnBattleResultAccepted.Broadcast(this, InDamageResult);
  }
}

/**End IARAttackable implementation */

/**Start IARAttackerInterface implementation */
#pragma region IARAttackerInterface implementation

void AARRangerCharacter::OnNotifyAttackResult_Success(const ARRanger::Battle::FARAttackNotifyParameter& InNotifyParams)
{
  // ヒット音を再生
  OnAttackHitNotify();

  if (OnPlayerHit.IsBound())
  {
    OnPlayerHit.Broadcast(GetActorLocation());
  }

  // 攻撃時フォースフィードバックエフェクトを再生
  if (APlayerController* PC = Cast<APlayerController>(GetController()))
  {
    if (FFE_Attack)
    {
      FForceFeedbackParameters Params;
      Params.bLooping = false;
      Params.bIgnoreTimeDilation = false;
      Params.Tag = FName("AttackFeedback");

      PC->ClientPlayForceFeedback(FFE_Attack, Params);
    }
  }
}

#pragma endregion IARAttackerInterface implementation
/**End IARAttackerInterface implementation */

void AARRangerCharacter::OnAttackAbilityStarted()
{
  if (AttackAbilityStartDelegate.IsBound())
  {
    AttackAbilityStartDelegate.Broadcast();
  }
}

void AARRangerCharacter::OnAttackAbilityEnded()
{
  if (AttackAbilityEndDelegate.IsBound())
  {
    AttackAbilityEndDelegate.Broadcast();
  }
}

void AARRangerCharacter::SetCameraRig(ECameraRigType InType)
{
  CameraRigType = InType;
  CameraRigChangeEvent.Broadcast(CameraRigType);
}

ECameraRigType AARRangerCharacter::GetCameraRig() const
{
  return CameraRigType;
}

void AARRangerCharacter::OnTargetSnapped(const FVector& InNewPosition, const FRotator& InNewRotation)
{
  // TODO
  SetActorLocation(InNewPosition);
  SetActorRotation(InNewRotation);
}

void AARRangerCharacter::OnPlayerDeadStarted(AActor* PlayerActor)
{
  DisableMovementAndCollision();

  // Blueprint call
  {
    K2_OnPlayerDeadStarted();
  }

}

void AARRangerCharacter::OnPlayerDeadEnded(AActor* PlayerActor)
{
  if (UARPawnInitComponent* PIC = GetComponentByClass<UARPawnInitComponent>())
  {
    PIC->UninitializeAbilitySystem();
    PIC->UninitializeChargeAttack();
  }

  if (OnPlayerDead.IsBound())
  {
    OnPlayerDead.Broadcast();
  }

  // Blueprint call
  {
    K2_OnPlayerDeadFinished();
  }
}

void AARRangerCharacter::DisableMovementAndCollision()
{
  if (AController* controller = GetController())
  {
    controller->SetIgnoreMoveInput(true);
  }

  UCapsuleComponent* capsuleComp = GetCapsuleComponent();
  capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  capsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

  UCharacterMovementComponent* moveComp = GetCharacterMovement();
  moveComp->StopMovementImmediately();
  moveComp->DisableMovement();
}

void AARRangerCharacter::OnHealthChanged(AActor* InInstigator, float InChangeValue, bool bIsDead)
{
  K2_OnHealthChanged(InInstigator, InChangeValue, bIsDead);
}

void AARRangerCharacter::OnAbilityCostHandled(UARAbilityCostComponent* InAbilityCostComponent, FGameplayTag AbilityCostTag, float InOldResourceValue, float InNewResourceValue, bool bAbilityCostHandled)
{
  // TODO
}

bool AARRangerCharacter::TryApplyAbilityCost(const FGameplayTag& InAbilityCostTag, float InAbilictCostChangeNum)
{
  bool bSuccess = false;
  if (AbilityCostComponent != nullptr)
  {
    AbilityCostComponent->HandleAbilityCostChanged(InAbilityCostTag, InAbilictCostChangeNum, bSuccess);
  }

  return bSuccess;
}

bool AARRangerCharacter::GetIsLockedOn() const
{ 
  return LockOnComponent->GetIsLockedOn(); 
}

void AARRangerCharacter::OnLanded(const FHitResult& Hit)
{
  if (UARRangerAnimInstance* animInst = ::Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    const float fallingTime = animInst->InFallingTime;
    // 一定時間以上落下していたら着地時に振動を発生
    if (fallingTime >= MinFallTimeForFeedback)
    {
      if (APlayerController* PC = Cast<APlayerController>(GetController()))
      {
        if (FFE_Landed != nullptr)
        {
          FForceFeedbackParameters Params;
          Params.bLooping = false;
          Params.bIgnoreTimeDilation = false;
          Params.Tag = FName("LandedFeedback");

          PC->ClientPlayForceFeedback(FFE_Landed, Params);
        }
      }
    }
  }
}

void AARRangerCharacter::LaunchCharacter_Ext(const FVector& InLaunchDirection, double InLaunchPower)
{
  LaunchCharacter(InLaunchDirection * InLaunchPower, false, true);

  // TODO Add force feedback
}

void AARRangerCharacter::Jump()
{
  Super::Jump();

  if (OnJumpedDelegate.IsBound())
  {
    OnJumpedDelegate.Broadcast();
  }

}
void AARRangerCharacter::StopJumping()
{
  Super::StopJumping();

  if (OnJumpStoppedDelegate.IsBound())
  {
    OnJumpStoppedDelegate.Broadcast();
  }
}