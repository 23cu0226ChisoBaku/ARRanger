#include "ARRangerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ARRangerAnimInstance.h"
#include "AttackBaseComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LockOnComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "PunchCameraShake.h"
#include "Player/ARPlayerState.h"
#include "PlayerComponents/AttractSpecialAttackComponent.h"
#include "Sound/SoundBase.h"

#include "Pawn/ARPawnInitComponent.h"
#include "Character/ARHealthComponent.h"
#include "Character/ARAbilityCostComponent.h"
#include "Pawn/ARPawnInitComponent.h"
#include "GameFramework/ForceFeedbackEffect.h"

#include "MLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

namespace
{
  static const float MAX_INPUT_VALUE = 1.0f;
}

AARRangerCharacter::AARRangerCharacter()
  : currentClimbSurface(nullptr)
  , bIsClimbed(false)
{
  // カプセルサイズを設定
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
  
  // コントローラーの回転をオフに
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  // プレイヤーの回転をオンに
  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

  // CharacterMovementの値を設定
  GetCharacterMovement()->JumpZVelocity = 500.f;
  GetCharacterMovement()->AirControl = 0.35f;
  GetCharacterMovement()->MaxWalkSpeed = 500.f;
  GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
  GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
  GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

  // 各種コンポーネントを取得
  LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
  AttackBaseComp = CreateDefaultSubobject<UAttackBaseComponent>(TEXT("AttackBaseComponent"));

  HealthComponent = CreateDefaultSubobject<UARHealthComponent>(TEXT("HealthComponent"));
  HealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnVignetteEffectChanged);
  HealthComponent->OnDeadEventStarted.AddDynamic(this, &ThisClass::OnPlayerDeadStarted);
  HealthComponent->OnDeadEventFinished.AddDynamic(this, &ThisClass::OnPlayerDeadEnded);

  AbilityCostComponent = CreateDefaultSubobject<UARAbilityCostComponent>(TEXT("AbilityCostComponent"));
  AbilityCostComponent->OnAbilityCostApplied.AddDynamic(this, &ThisClass::OnAbilityCostHandled);

  CameraRigIndex = 0;
}

void AARRangerCharacter::PostInitializeComponents()
{
  Super::PostInitializeComponents();

}

void AARRangerCharacter::BeginPlay()
{
  Super::BeginPlay();

  // ワールド内のInsekiClimbingObjectをすべて取得し、バインドする（デモ用）
  TArray<AActor*> ClimbSurfaces;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInsekiClimbingObject::StaticClass(), ClimbSurfaces);

  for (AActor* Actor : ClimbSurfaces)
  {
    if (AInsekiClimbingObject* ClimbObjects = Cast<AInsekiClimbingObject>(Actor))
    {
      if (ClimbObjects->ClimbTrigger)
      {
        ClimbObjects->ClimbTrigger->OnComponentBeginOverlap.AddDynamic(this, &AARRangerCharacter::OnClimbSurfaceOverlap);
      }
    }
  }

  // 麦
  LandedDelegate.AddDynamic(this, &AARRangerCharacter::LandedToGround);

  // 麦
  GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AARRangerCharacter::OnMagneticForceFieldBeginOverlap);
  GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AARRangerCharacter::OnMagneticForceFieldEndOverlap);
  GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AARRangerCharacter::OnMagnetizedObjectHit);

  // TODO Temporary
  if (AARPlayerState* ARPS = GetPlayerState<AARPlayerState>())
  {
    if (UARPawnInitComponent* PIC = ::Cast<UARPawnInitComponent>(GetComponentByClass(UARPawnInitComponent::StaticClass())))
    {
      PIC->InitializeAbilitySystem(ARPS->GetARAbilitySystemComponent(), ARPS); 

      PIC->InitializeChargeAttack(ARPS->GetARChargeAttackComponent());
    }
  }

  attractSpecialAttackComponent = FindComponentByClass<UAttractSpecialAttackComponent>();

  SetMagnetismType(EARMagnetismType::Repulsion);

  // Call this once in BeginPlay
  K2_OnTransformed(RepulsionMesh, EARMagnetismType::Repulsion);
}

// 麦
void AARRangerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  // 麦
  LandedDelegate.RemoveDynamic(this, &AARRangerCharacter::LandedToGround);

  GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &AARRangerCharacter::OnMagneticForceFieldBeginOverlap);
  GetCapsuleComponent()->OnComponentEndOverlap.RemoveDynamic(this, &AARRangerCharacter::OnMagneticForceFieldEndOverlap);
  GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &AARRangerCharacter::OnMagnetizedObjectHit);

  Super::EndPlay(EndPlayReason);
}

void AARRangerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  Super::SetupPlayerInputComponent(PlayerInputComponent);
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

  if (bCanTargetSnap)
  {
    SnapToTarget(DeltaTime);
  }

  bool isLockedOn = LockOnComponent->GetIsLockedOn();
  AActor* Target = LockOnComponent->GetLockedOnTarget();

  // 落下中に処理
  UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance());
  if (MyAnim != nullptr)
  {
    if (MyAnim->IsFalled)
    {
      MyAnim->InFallingTime += DeltaTime;
    }
    else
    {
      // 一定時間以上落下していたら着地時に振動を発生
      if (MyAnim->InFallingTime >= MinFallTimeForFeedback)
      {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
          if (FFE_Landed)
          {
            FForceFeedbackParameters Params;
            Params.bLooping = false;
            Params.bIgnoreTimeDilation = false;
            Params.Tag = FName("LandedFeedback");
  
            PC->ClientPlayForceFeedback(FFE_Landed, Params);
          }
        }
      }
  
      // 落下時間をリセット
      MyAnim->InFallingTime = 0.0f;
    }
  }

  // ロックオン中に処理
  if (isLockedOn && Target)
  {
    FVector ToTarget = Target->GetActorLocation() - GetActorLocation();
    FRotator TargetRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
    TargetRotation.Pitch = 0.f;
    TargetRotation.Roll = 0.f;

    // ターゲットに向けて回転
    SetActorRotation(TargetRotation);

    // コントローラーも回転する
    if (Controller)
    {
      FRotator CurrentControlRot = Controller->GetControlRotation();

      // 補間も入れて滑らかに回転させる
      FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetRotation, DeltaTime, 5.0f);
      Controller->SetControlRotation(NewControlRot);
    }
  }

  // 今フレームで重なってるコンポーネントを取得
  TArray<UPrimitiveComponent*> OverlappingComps;
  GetOverlappingComponents(OverlappingComps);

  for (UPrimitiveComponent* Comp : OverlappingComps)
  {
    if (AInsekiClimbingObject* Surface = Cast<AInsekiClimbingObject>(Comp->GetOwner()))
    {
      // OnClimbSurfaceOverlap を手動で呼ぶ
      StartClimbing(Surface);
    }
  }

  // 引力クライム中に処理
  if (bIsClimbed)
  {
    const float ClimbSpeed = 2100.0f; // 上昇速度
    AddActorWorldOffset(FVector(0, 0, ClimbSpeed * DeltaTime), false);

    // 壁回転処理
    // 足元の位置（Capsuleの底の位置）
    UCapsuleComponent* Capsule = GetCapsuleComponent();
    FVector ActorLocation = GetActorLocation();
    float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
    // 壁に対して垂直な向きに少しめり込むようにして設定
    FVector FootPosition = ActorLocation -wallNormal * (HalfHeight - 5.0f);

    FVector Start = FootPosition;
    FVector End = Start - wallNormal * 7.0f;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

    // ライントレースで壁を判定
    // 壁がないか、または引力クライム中に斥力状態に変身したらクライムを解除
    if (!bHit || (GetMagnetismType() != EARMagnetismType::Attraction))
    {
      // クライム解除＋ジャンプ処理
      bIsClimbed = false;
      GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
      GetCharacterMovement()->bOrientRotationToMovement = true;
      // AnimInstance側のフラグも下げる
      MyAnim->IsClimbing = false;

      // 少し上方向にジャンプさせる
      LaunchCharacter(FVector(0.0f, 0.0f, 700.0f), true, true);
    }
  }
}

void AARRangerCharacter::OnClimbSurfaceOverlap(
  UPrimitiveComponent* OverlappedComp,
  AActor* OtherActor,
  UPrimitiveComponent* OtherComp,
  int32 OtherBodyIndex,
  bool bFromSweep,
  const FHitResult& SweepResult)
{
  if (OtherActor == this)
  {
    AInsekiClimbingObject* Surface = Cast<AInsekiClimbingObject>(OverlappedComp->GetOwner());
    if (Surface)
    {
      StartClimbing(Surface);
    }
  }
}

void AARRangerCharacter::DoMove(float Right, float Forward)
{
  // コントローラーがない、引き寄せ中または攻撃中なら処理しない
  if (GetController() == nullptr || bIsAttracted || bIsAttacked || bIsStrongAttacked)
  {
    return;
  }

  // 入力値の絶対値をチェックしてデッドゾーン以下は0に
  // Modified By MAI
  const float radiusSquared = FMath::Square(Forward) + FMath::Square(Right);
  const float moveDeadZoneSquared = FMath::Square(FMath::Max(0.0f, MoveDeadZone));

  // デッドゾーン以下
  if (radiusSquared <= moveDeadZoneSquared)
  {
    return;
  }
  
  const float realMinInput = FMath::Min(MinInput, MAX_INPUT_VALUE);
  // インプット閾値レベル
  const TArray<float> inputThresholdLevel{ 
                        realMinInput,       // LV1  : 最小入力値
                        MAX_INPUT_VALUE,    // LVMax: 最大入力値
                      };

  for (int32 idx = 0; idx < inputThresholdLevel.Num(); ++idx)
  {
    // インプット閾値まで補正する
    const float inputLevelValue = inputThresholdLevel[idx];
    const float inputLevelValueSquared = FMath::Square(inputLevelValue);
    if (radiusSquared < inputLevelValueSquared)
    {
      const float inputModifier = inputLevelValue / FMath::Sqrt(radiusSquared);
      Forward *= inputModifier;
      Right   *= inputModifier;

      break;
    }   
  }

  if (!bIsClimbed)
  {
    const FRotator Rotation = GetController()->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, Forward);
    AddMovementInput(RightDirection, Right);
  }
  else
  {
    // 壁に触れていなければ処理しない
    if (!currentClimbSurface)
    {
      UE_LOG(LogTemp, Warning, TEXT("currentClimbSurface is null!"));
      return;
    }

    // 移動軸を変更する
    if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
    {
      MyAnim->ClimbUpSpeed = Forward;
      MyAnim->ClimbRightSpeed = Right;
    }
  }
}

void AARRangerCharacter::StartClimbing(AInsekiClimbingObject* ClimbActor)
{
  // クライム中でない、引力クライムオブジェクトに触れていない、または引力状態でないなら処理しない
  if (bIsClimbed || !ClimbActor || GetMagnetismType() != EARMagnetismType::Attraction)
  {
    return;
  }
    
  // 引力クライムフラグを上げる
  bIsClimbed = true;
  currentClimbSurface = ClimbActor;
  // AnimInstance側のフラグも上げる
  if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    MyAnim->IsClimbing = true;
  }

  GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
  GetCharacterMovement()->bOrientRotationToMovement = false;

  // 壁があるかを判定
  FHitResult HitResult;
  FVector Start = GetActorLocation();
  FVector End = Start + GetActorForwardVector() * 100.0f;

  FCollisionQueryParams Params;
  Params.AddIgnoredActor(this);

  const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
  
  if (bHit)
  {
    // 壁の法線を保存
    wallNormal = HitResult.ImpactNormal;
  }
}

void AARRangerCharacter::StopClimbing()
{
  // 引力クライム中でないなら処理しない
  if (!bIsClimbed)
  {
    return;
  }
    
  // 引力クライムフラグを下げる
  bIsClimbed = false;
  currentClimbSurface = nullptr;
  GetCharacterMovement()->bOrientRotationToMovement = true;
  // AnimInstance側のフラグも下げる
  if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    MyAnim->IsClimbing = false;
  }

  // 回転を元に戻す
  SetActorRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));
  GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void AARRangerCharacter::DoLook(float Yaw, float Pitch)
{
  if (GetController() != nullptr)
  {
    // カメラを回転
    AddControllerYawInput(Yaw);
    AddControllerPitchInput(Pitch);
  }
}

void AARRangerCharacter::DoJumpStart()
{
  // 攻撃中・引き寄せ中は処理しない
  if (bIsAttacked || bIsStrongAttacked || bIsAttracted)
  {
    return;
  }

  // 引力クライムを解除
  if (bIsClimbed)
  {
    StopClimbing();
  }

  // 麦
  if (!bIsJumping)
  {
    TSharedRef<ARRanger::INotifyHandlerInterface> notifyHandler = GetNotifyHandlerRef();
    notifyHandler->OnJump();
    bIsJumping = true;
  }
}

void AARRangerCharacter::DoJumpEnd()
{
  // ジャンプをやめる
  StopJumping();
}

void AARRangerCharacter::OnAttractionCompleted()
{
  // 引き寄せ完了フラグを立てる
  SetIsApproachedEnemy(true);
  UE_LOG(LogTemp, Warning, TEXT("Attraction Punch Start!"));
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

void AARRangerCharacter::OnDeadEnemy()
{
  // ばぐのおきないよう
}

void AARRangerCharacter::OnAttackHitNotify()
{
  // プレイヤー内でのみ扱いたいのでこちらで攻撃のコールバック
  TSharedRef<ARRanger::INotifyHandlerInterface> notifyHandler = GetNotifyHandlerRef();
  notifyHandler->OnAttack();
}

void AARRangerCharacter::Transform()
{
  // 攻撃中・引き寄せ中は処理しない
  if (bIsAttacked || bIsStrongAttacked || bIsAttracted)
  {
    return;
  }

  const EARMagnetismType prevType = GetMagnetismType();
  // 現在と別のモードに変身
  SetMagnetismType(
    (prevType == EARMagnetismType::Repulsion)
    ? EARMagnetismType::Attraction
    : EARMagnetismType::Repulsion);

  const EARMagnetismType curtType = GetMagnetismType();

  // メッシュを別モードに変更
  USkeletalMesh* NewMesh = (curtType == EARMagnetismType::Repulsion)
    ? RepulsionMesh
    : AttractionMesh;

  if (NewMesh)
  {
    GetMesh()->SetSkeletalMesh(NewMesh);
    K2_OnTransformed(NewMesh, curtType);
  }

  // 変身エフェクトを再生
  if (TransformEffect)
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

bool AARRangerCharacter::CanSpecialAttractAttack()
{
  // 攻撃・強攻撃中、引き寄せ中、ジャンプ中、引力クライム中はfalseを返す
  if (bIsAttacked || bIsStrongAttacked || bIsAttracted || bIsJumping || bIsClimbed)
  {
    return false;
  }

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
  LockOnComponent->SetIsLockedOn(false);

  // キャラクターの速度をあらかじめ0にしておく
  APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
  APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
  if (PlayerPawn)
  {
    if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(PlayerPawn->GetMovementComponent()))
    {
      MoveComp->StopMovementImmediately();
    }
  }

  // To Do：必殺技の途中に攻撃を食らわないよう、プレイヤーを無敵状態にしておく


  if (attractSpecialAttackComponent != nullptr)
  {
    attractSpecialAttackComponent->StartSpecialAttract();
  }
}

EARMagnetismType AARRangerCharacter::GetCurrentARType()
{
  return GetMagnetismType();
}

void AARRangerCharacter::ResetIsAttacked()
{
  SetIsAttacked(false);
  SetIsStrongAttacked(false);
  UE_LOG(LogTemp, Warning, TEXT("ResetAttack → IsAttacked = false"));
}

void AARRangerCharacter::SetIsBattledInAnimInstance(const bool IsBattled)
{
  if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
  {
    MyAnim->bIsBattled = IsBattled;
    UE_LOG(LogTemp, Warning, TEXT("IsBattled : %s"), IsBattled ? TEXT("True") : TEXT("False"));
    UE_LOG(LogTemp, Warning, TEXT("bIsBattled : %s"), MyAnim->bIsBattled ? TEXT("True") : TEXT("False"));

    // TODO We should not let auto generation system in HealtComponent
    if (HealthComponent != nullptr)
    {
      HealthComponent->SetAutoRegenerationEnable(!IsBattled);
    }
  
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

void AARRangerCharacter::OnMagneticForceFieldBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
  {
    Physics_RegisterMagneticTask(this, magnetizableObj);
  }
}

void AARRangerCharacter::OnMagneticForceFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
  {
    Physics_UnregisterMagneticTask(this, magnetizableObj);
  }
}

void AARRangerCharacter::OnMagnetizedObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
  if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
  {
    Physics_RegisterMagneticTask_Once(this, magnetizableObj);
  }
}

void AARRangerCharacter::OnRepulsionEvaluated(const FARMagneticForceResult& Result)
{
  MDebug::LogError("OnRepulsionEvaluated");
  LaunchCharacter(Result.FinalForce, true, false);
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
  if (HealthComponent != nullptr)
  {
    // Value of damage is positive. Make it negative
    const float HPChangeValue = -InDamageResult.FinalDamage;
    HealthComponent->HandleHealthChange(InDamageResult.Instigator, HPChangeValue);

    if (HealthComponent->GetHealth() <= 0.0f)
    {
      HealthComponent->HandleOutOfHealth(this);
    }
    else
    {
      FVector knockbackDir = InDamageResult.FinalLaunchDirection;
      knockbackDir.Z = 0;
      knockbackDir.Normalize();

      LaunchCharacter(knockbackDir * 400.0, false, true);
    }
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

// TODO Temporary blueprint callable function 
void AARRangerCharacter::OnPunchStarted()
{
  // TODO
  if (bReadyToTargetSnap && !TargetSnapInput.IsNearlyZero())
  {
    SearchTargetToSnap();
  }

  bReadyToTargetSnap = false;
}

void AARRangerCharacter::OnPunchEnded()
{
  m_snapTimeCnt = 0.0f;
  bReadyToTargetSnap = false;
  TargetSnapInput = FVector2D::ZeroVector;
  TargetToSnap = nullptr;
}

void AARRangerCharacter::RotateCharacter_Charge(float Yaw)
{
  if (!bIsHolding || FMath::IsNearlyZero(Yaw))
  {
    return;
  }

  const float RotateOffsetMax = 60.0f;
  const FRotator curtPlayerDir_Rot = GetActorRotation();

  FRotator nextPlayerDir_Rot = curtPlayerDir_Rot + FRotator{0.0, (double)Yaw, 0.0};
  if (FVector::DotProduct(FaceDir_HoldStart, nextPlayerDir_Rot.Vector()) < FMath::Cos(FMath::DegreesToRadians(RotateOffsetMax)))
  {
    nextPlayerDir_Rot = FaceDir_HoldStart.Rotation() + FMath::Sign(Yaw) * FRotator{0.0, (double)RotateOffsetMax, 0.0}; 
  }

  SetActorRotation(nextPlayerDir_Rot);

}

// Call if we start charge kick
void AARRangerCharacter::OnHoldStarted(const FGameplayTag& InActivatedAbilityTag)
{
  FaceDir_HoldStart = GetActorForwardVector();
  bIsHolding = true;
}

void AARRangerCharacter::OnHoldEnded()
{
  bIsHolding = false;
}

void AARRangerCharacter::UpdateTargetSnap(const FVector2D& InputDir)
{
  if (InputDir.IsNearlyZero())
  {
    TargetSnapInput = FVector2D::ZeroVector;
    bReadyToTargetSnap = false;
    return;
  }

  if (bCanTargetSnap)
  {
    return;
  }

  bReadyToTargetSnap = true;
  bCanTargetSnap = false;
  TargetSnapInput = InputDir;
}

void AARRangerCharacter::SearchTargetToSnap()
{
  TargetToSnap = nullptr;
  TargetSnapInput.Normalize();

  const FVector curtPlayerDir = GetActorForwardVector();

  const FRotator curtCtrlRotation = GetController()->GetControlRotation();
  const FRotator yawRotation{0.0, curtCtrlRotation.Yaw, 0.0};
  const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
  const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
  const FVector targetPlayerDir = ((forwardDirection * TargetSnapInput.Y) + (rightDirection * TargetSnapInput.X)).GetSafeNormal();
  const FVector startLoc = GetActorLocation();
  const FVector endLoc = startLoc + targetPlayerDir * TargetSnapDetectLength;

  float radius = 200.f;
  if (UCapsuleComponent* capsule = GetCapsuleComponent())
  {
    radius = capsule->GetScaledCapsuleHalfHeight();
  }

  // TODO
  TArray< TEnumAsByte<EObjectTypeQuery> > objTypes{}; 
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
  TArray<AActor*> ignoreActors{};
  ignoreActors.Add(this);

  TArray<FHitResult> outResults{};

  const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
                                            this,
                                            startLoc,
                                            endLoc,
                                            radius,
                                            objTypes,
                                            false,        // bTraceComplex
                                            ignoreActors,
                                            EDrawDebugTrace::Persistent,
                                            outResults,
                                            true          // bIgnoreSelf
                                          ); 
  
  if (bHit)
  {
    const FVector playerLoc = startLoc;
    for (const FHitResult& hitResult : outResults)
    {
      AActor* hitActor = hitResult.GetActor();
      if (hitActor == nullptr)
      {
        continue;
      }

      // Ignore actors that hit by hemisphere behind player's face direction
      const FVector dirToTarget = (hitActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
      if (FVector::DotProduct(dirToTarget, targetPlayerDir) < 0.0f)
      {
        continue;
      }

      if (hitActor->GetClass()->ImplementsInterface(UARAttackable::StaticClass()))
      {
        IARAttackable* attackable = ::Cast<IARAttackable>(hitActor);
        if (attackable == nullptr)
        {
          continue;
        }

        const float curtHitResultDistanceSquared = (playerLoc - hitResult.GetActor()->GetActorLocation()).SquaredLength();
        if (TargetToSnap != nullptr)
        {
          // Find min distance to player
          const float curtMinDistanceSquared = (playerLoc - TargetToSnap->GetActorLocation()).SquaredLength();
          if (curtMinDistanceSquared > curtHitResultDistanceSquared)
          {
            TargetToSnap = hitResult.GetActor();
            TargetPrimitiveComp = hitResult.GetComponent();
            const FVector relativeImpactPoint = hitResult.ImpactPoint - TargetPrimitiveComp->GetComponentLocation();
            // We do not use Z-component of impact point
            TargetImpactPoint_Local = FVector{relativeImpactPoint.X, relativeImpactPoint.Y, 0.0};
          }   
        }
        else
        {
          TargetToSnap = hitResult.GetActor();
          TargetPrimitiveComp = hitResult.GetComponent();
          const FVector relativeImpactPoint = hitResult.ImpactPoint - TargetPrimitiveComp->GetComponentLocation();
          // We do not use Z-component of impact point
          TargetImpactPoint_Local = FVector{relativeImpactPoint.X, relativeImpactPoint.Y, 0.0};
        }

        bCanTargetSnap = true;
      }
    }

    if (bCanTargetSnap)
    {
      m_startSnapPlayerLocation = GetActorLocation();
      m_startSnapPlayerRotation = GetActorRotation();
    }
  }
}

void AARRangerCharacter::SnapToTarget(float DeltaTime)
{
  FVector newLocation = GetActorLocation();
  FRotator newRotation = GetActorRotation();

  if ((TargetToSnap != nullptr) && 
      !FMath::IsNearlyZero(SnapTimeInterval) && 
      (m_snapTimeCnt < SnapTimeInterval)
    )
  {
    const float alphaMin = 0.0f;
    const float alphaMax = 1.0f;

    m_snapTimeCnt += DeltaTime;
    const float RotateInterpInterval = SnapTimeInterval / 2.0f;
    const FRotator faceToTargetRot = (TargetToSnap->GetActorLocation() - GetActorLocation()).Rotation();
    const float rotLerpAlpha = FMath::Clamp((m_snapTimeCnt / RotateInterpInterval), alphaMin, alphaMax);
    newRotation = FMath::InterpCircularOut(m_startSnapPlayerRotation, faceToTargetRot, rotLerpAlpha);
    
    // Calculate new location 
    const FVector playerToTargetOffset = TargetImpactPoint_Local.GetSafeNormal() * GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector newTargetLocation = TargetToSnap->GetActorLocation() + TargetImpactPoint_Local + playerToTargetOffset;
    const FVector newTargetLocation_UsePlayerZ = FVector{newTargetLocation.X, newTargetLocation.Y, GetActorLocation().Z};
    const float locLerpAlpha = FMath::Clamp((m_snapTimeCnt / SnapTimeInterval), alphaMin, alphaMax);
    newLocation = FMath::InterpCircularIn(m_startSnapPlayerLocation, newTargetLocation_UsePlayerZ, locLerpAlpha);
  }
  // Stop snapping
  else
  {
    bCanTargetSnap = false;
    m_snapTimeCnt = 0.0f;
    TargetSnapInput = FVector2D::ZeroVector;
    TargetToSnap = nullptr;
  }

  // Update Player
  SetActorLocation(newLocation);
  SetActorRotation(newRotation);

}

void AARRangerCharacter::OnPlayerDeadStarted(AActor* PlayerActor)
{
  DisableMovementAndCollision();

  K2_OnPlayerDeadStarted();
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

  K2_OnPlayerDeadFinished();

}

void AARRangerCharacter::DisableMovementAndCollision()
{
  if (AController* controller = GetController())
  {
    controller->SetIgnoreMoveInput(true);
  }

  UCapsuleComponent* capsuleComp = GetCapsuleComponent();
  check(capsuleComp != nullptr);

  capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  capsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

  UCharacterMovementComponent* moveComp = GetCharacterMovement();
  moveComp->StopMovementImmediately();
  moveComp->DisableMovement();

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

void AARRangerCharacter::ResetCamera()
{
  CameraRigIndex = 2;
}