#include "Presentation/ARPlayerPresenter.h"

#include "GameFramework/PlayerController.h"
#include "Character/ARRangerCharacter.h"
#include "Character/ARHealthComponent.h"
#include "PlayerComponents/LockOnComponent.h"
#include "PlayerComponents/CameraRouterComponent.h"
#include "BattleSystem/IARAttackable.h"
#include "Components/CapsuleComponent.h"

#include "Magnetic/InsekiClimbingObject.h"

#include "Kismet/KismetSystemLibrary.h"

namespace 
{
  double CalcLockTargetPriority(const FVector& TargetDirNorm, const FVector& BaseDirNorm);
}

FARPlayerModel::FARPlayerModel()
  : HealthComponent{nullptr}
  , LockOnComponent{nullptr}
  , ChargeStartFaceDir{EForceInit::ForceInitToZero}
  , ClimbSurfaceNormal{EForceInit::ForceInitToZero}
  , TargetSnapInputDirection{EForceInit::ForceInitToZero}
  , LaunchPower{400.0}
  , ChargeRotateHalfRange{60.0}
  , TargetSnapDetectMaxRange{200.0}
  , SnapTimeInterval{0.2f}
  , SnapTimeCounter{0.0f}
  , SnapTargetActor{nullptr}
  , LockOnTarget{nullptr}
  , CurrentRigType{ECameraRigType::FreeAngle}
  , bIsCharging{false}
  , bIsInAir{false}
  , bIsClimbing{false}
  , bIsInComboAction{false}
  , bCanUpdateSnapMovement{false}
  , bIsReadyToSearchSnapTarget{false}
  , bCanLockOn{true}
  , bIsLockingOn{false}
  , m_switchLockTargetState{None}
{ }

void FARPlayerModel::Initialize(AARRangerCharacter* InViewCharacter)
{
  check(InViewCharacter != nullptr);

  HealthComponent = static_cast<UARHealthComponent*>(InViewCharacter->GetComponentByClass(UARHealthComponent::StaticClass()));
  LockOnComponent = static_cast<ULockOnComponent*>(InViewCharacter->GetComponentByClass(ULockOnComponent::StaticClass()));
  CameraRouter = static_cast<UCameraRouterComponent*>(InViewCharacter->GetComponentByClass(UCameraRouterComponent::StaticClass()));

  LockOnTarget.Reset();

}

void FARPlayerModel::Reset()
{
  HealthComponent = nullptr;
  LockOnComponent = nullptr;
  CameraRouter = nullptr;
}

void FARPlayerModel::SetCameraRig(ECameraRigType Type)
{
  if (CurrentRigType == Type)
  {
    return;
  }

  ECameraRigType oldRig = CurrentRigType;
  CurrentRigType = Type;
  if (RigChangeEvent.IsBound())
  {
    RigChangeEvent.Broadcast(oldRig, CurrentRigType);
  }
}

void FARPlayerModel::UpdateLockOnTargets(AActor* UserActor, const TArray<AActor*>& InTargets)
{
  if (GEngine)
  {
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Update Lock On Targets: %d"), InTargets.Num()));
  }

  const bool bIsLockOnFunctional = bIsLockingOn && (UserActor != nullptr);
  if (!bIsLockOnFunctional)
  {
    return;
  }
  
  AActor* currentTarget = LockOnTarget.Get();
  bool bShouldTargetUpdate = false;

  // Try updating target.
  // Otherwise we find a new target
  if (currentTarget == nullptr)
  {
    if (InTargets.Num() == 0)
    {
      return;
    }

    TArray<AActor*> allTargets{InTargets};

    // Get the nearest target to BaseDirection(Biggest dot value)
    // After sort, the first element in the array should be the nearest target
    auto sortGreaterDotValPred = 
    [
      UserActorLocation = UserActor->GetActorLocation(),
      BaseDirection = UserActor->GetActorForwardVector()
    ]
    (const AActor& InElementA, const AActor& InElementB) -> bool
    {
      const FVector ALocation = InElementA.GetActorLocation();
      const FVector ADirNorm2D = (ALocation - UserActorLocation).GetSafeNormal2D();
      const FVector BLocation = InElementB.GetActorLocation();
      const FVector BDirNorm2D = (BLocation - UserActorLocation).GetSafeNormal2D();

      const double ADotVal = FVector::DotProduct(ADirNorm2D, BaseDirection);
      const double BDotVal = FVector::DotProduct(BDirNorm2D, BaseDirection);

      return ADotVal > BDotVal;
      
    };

    allTargets.Sort(sortGreaterDotValPred);
    check(allTargets.Num() != 0);
    currentTarget = allTargets[0];

    bShouldTargetUpdate = true;
  }
  else if (!IsTargetValidToLockOn(currentTarget))
  {
    currentTarget = nullptr;

    bShouldTargetUpdate = true;
  }
  else
  {
    if (InTargets.Num() == 0)
    {
      return;
    }

    // Priority check of all targets
    if (m_switchLockTargetState != None)
    {
      // Sort all targets(Include current target)
      TArray<AActor*> allTargets{InTargets};
      if (!allTargets.Contains(currentTarget))
      {
        allTargets.Add(currentTarget);
      }

      // TODO Need comment
      /**
       *
       * 
       */
      auto sortLockTargetsPred = 
      [
        UserActorLocation = UserActor->GetActorLocation(),
        CurrentTargetDirNorm2D = (currentTarget->GetActorLocation() - UserActor->GetActorLocation()).GetSafeNormal2D()
      ]
      (const AActor& InElementA, const AActor& InElementB) -> bool
      {
        const FVector ALocation = InElementA.GetActorLocation();
        const FVector ADirNorm2D = (ALocation - UserActorLocation).GetSafeNormal2D();
        const FVector BLocation = InElementB.GetActorLocation();
        const FVector BDirNorm2D = (BLocation - UserActorLocation).GetSafeNormal2D();

        const double APriority = CalcLockTargetPriority(ADirNorm2D, CurrentTargetDirNorm2D);
        const double BPriority = CalcLockTargetPriority(BDirNorm2D, CurrentTargetDirNorm2D);

        return APriority < BPriority;
      };

      allTargets.Sort(sortLockTargetsPred);

      int32 newTargetIdx = allTargets.IndexOfByKey(currentTarget);
      check(newTargetIdx != INDEX_NONE);
      switch(m_switchLockTargetState)
      {
        case Left:
        {
          newTargetIdx -= 1;
        }
        break;

        case Right:
        {
          newTargetIdx += 1;
        }
        break;
      }

      // Consume switch state
      ConsumeSwitchTargetState();

      newTargetIdx = FMath::Clamp(newTargetIdx, 0, allTargets.Num() - 1);
      if (currentTarget != allTargets[newTargetIdx])
      {
        currentTarget = allTargets[newTargetIdx];
        bShouldTargetUpdate = true;
      }
    }
  }


  if (bShouldTargetUpdate)
  {
    LockOnTarget = currentTarget;
    
    if (LockOnTargetUpdateEvent.IsBound())
    {
      LockOnTargetUpdateEvent.Broadcast(currentTarget);
    }
    // Since it is lock target state currently,
    // switch to unlock target state
    if (currentTarget == nullptr)
    {
      ToggleLockOn();
    }
  }
  
}

void FARPlayerModel::ToggleLockOn()
{
  // Switch lock on state
  bIsLockingOn = !bIsLockingOn;
  if (bIsLockingOn)
  {
    LockTargetInternal();
  }
  else
  {
    UnlockTargetInternal();
  }
}

void FARPlayerModel::SwitchLockTarget_Left()
{
  m_switchLockTargetState = Left;
}

void FARPlayerModel::SwitchLockTarget_Right()
{
  m_switchLockTargetState = Right;
}

void FARPlayerModel::LockTargetInternal()
{
  // SetCameraRig(ECameraRigType::LockOn);
}

void FARPlayerModel::UnlockTargetInternal()
{
  SetCameraRig(ECameraRigType::Default);
}

bool FARPlayerModel::IsTargetValidToLockOn(const AActor* InTarget) const
{
  return InTarget != nullptr;
}

void FARPlayerModel::ConsumeSwitchTargetState()
{
  m_switchLockTargetState = None;
}

void UARPlayerPresenter::Initialize(AARRangerCharacter* InViewCharacter, APlayerController* InPlayerController)
{
  check(InViewCharacter != nullptr);
  check(InPlayerController != nullptr);

  if (ViewCharacter == InViewCharacter)
  {
    return;
  }

  if (ViewCharacter != nullptr)
  {
    Deinitialize();
  }

  ViewCharacter = InViewCharacter;
  m_controller = InPlayerController;

  if (ViewCharacter != nullptr)
  {
    ViewCharacter->OnBattleResultAccepted.AddUObject(this, &ThisClass::HandleBattleResult);
    ViewCharacter->OnBattleStateChanged.AddUObject(this, &ThisClass::HandleBattleStateChange);
    ViewCharacter->OnTransformed.AddUObject(this, &ThisClass::HandleTransformedEvent);
    ViewCharacter->OnJumpedDelegate.AddUObject(this, &ThisClass::OnCharacterJumpStarted);
    ViewCharacter->OnJumpStoppedDelegate.AddUObject(this, &ThisClass::OnCharacterJumpStopped);
    ViewCharacter->AttackAbilityStartDelegate.AddUObject(this, &ThisClass::HandleAttackAbilityStarted);
    ViewCharacter->AttackAbilityEndDelegate.AddUObject(this, &ThisClass::HandleAttackAbilityEnded);

    ViewCharacter->LandedDelegate.AddDynamic(this, &ThisClass::OnGroundLanded);

    UCapsuleComponent* capsuleComp = ViewCharacter->GetCapsuleComponent();
    if (capsuleComp != nullptr)
    {
      capsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnClimbSurfaceOverlapBegan);
      capsuleComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnClimbSurfaceOverlapEnded);
      capsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnMagneticForceFieldBeginOverlap);
      capsuleComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnMagneticForceFieldEndOverlap);
      capsuleComp->OnComponentHit.AddDynamic(this, &ThisClass::OnMagnetizedObjectHit); 
    }

    Model.Initialize(ViewCharacter);

    if (Model.LockOnComponent != nullptr)
    {
      Model.LockOnComponent->OnLockOnDataUpdateEvent.AddUObject(this, &ThisClass::OnLockOnDataUpdated);
    }

    Model.RigChangeEvent.AddUObject(this, &ThisClass::OnCameraRigChanged);
    Model.LockOnTargetUpdateEvent.AddUObject(this, &ThisClass::OnLockOnTargetUpdated);

    // カメラリグを初期状態にする
    Model.SetCameraRig(ECameraRigType::Default);
  }
}

void UARPlayerPresenter::Deinitialize()
{
  if (ViewCharacter == nullptr)
  {
    return;
  }

  ViewCharacter->OnBattleResultAccepted.RemoveAll(this);
  ViewCharacter->OnBattleStateChanged.RemoveAll(this);
  ViewCharacter->OnTransformed.RemoveAll(this);
  ViewCharacter->OnJumpedDelegate.RemoveAll(this);
  ViewCharacter->OnJumpStoppedDelegate.RemoveAll(this);
  ViewCharacter->AttackAbilityStartDelegate.RemoveAll(this);
  ViewCharacter->AttackAbilityEndDelegate.RemoveAll(this);

  ViewCharacter->LandedDelegate.RemoveDynamic(this, &ThisClass::OnGroundLanded);

  UCapsuleComponent* capsuleComp = ViewCharacter->GetCapsuleComponent();
  if (capsuleComp != nullptr)
  {
    capsuleComp->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnClimbSurfaceOverlapBegan);
    capsuleComp->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnClimbSurfaceOverlapEnded);
    capsuleComp->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnMagneticForceFieldBeginOverlap);
    capsuleComp->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnMagneticForceFieldEndOverlap);
    capsuleComp->OnComponentHit.RemoveDynamic(this, &ThisClass::OnMagnetizedObjectHit);
  
  }

  Model.Reset();
}

void UARPlayerPresenter::Input_HandleLeftStick(double InX, double InY, double InDeadZone, double InMinInput)
{
  const bool bInputAllowed = (ViewCharacter != nullptr) && !Model.bIsClimbing;
  if (!bInputAllowed)
  {
    return;
  }

  // チャージ状態の回転処理
  if (Model.bIsCharging)
  {
    HandleCharacterChargeRotate(InX, InY);
  }
  // 吸着攻撃ターゲット更新処理
  else if (Model.bIsInComboAction)
  {
    HandleSnapTargetUpdate(InX, InY);
  }
  // 移動処理
  else
  {
    HandleCharacterMove(InX, InY, InDeadZone, InMinInput);
  }

}

void UARPlayerPresenter::Input_HandleTransform()
{
  if (ViewCharacter == nullptr)
  {
    return;
  }

  // TODO 変身条件を追加する予定
  const bool bCanTransform = true;
  if (!bCanTransform)
  {
    return;
  }

  ViewCharacter->Transform();
}

void UARPlayerPresenter::Input_HandleCameraReset()
{
  // TODO カメラ初期化条件を追加する予定
  const bool bCanResetCamera = true;
  if (!bCanResetCamera)
  {
    return;
  }

  Model.SetCameraRig(ECameraRigType::Reset);
}

void UARPlayerPresenter::Input_HandleLockOn()
{
  const bool bCanToggleLockOn = true;
  if (!bCanToggleLockOn)
  {
    return;
  }

  Model.ToggleLockOn();
}

void UARPlayerPresenter::Input_HandleRightStick(double InX, double InY)
{
  // Y軸入力は使わない
  (void)InY;

  if (!Model.bIsLockingOn)
  {
    return;
  }

  if (InX > 0.0)
  {
    Model.SwitchLockTarget_Right();
  }
  else if (InX < 0.0)
  {
    Model.SwitchLockTarget_Left();
  }
}

void UARPlayerPresenter::OnChargeStartHandled()
{
  Model.bIsCharging = true;
  if (ViewCharacter != nullptr)
  {
    Model.ChargeStartFaceDir = ViewCharacter->GetActorForwardVector();
  }
}

void UARPlayerPresenter::OnChargeEndHandled()
{
  Model.bIsCharging = false;
  Model.ChargeStartFaceDir = FVector::Zero();
}

void UARPlayerPresenter::HandleCharacterMove(double InX, double InY, double InDeadZone, double InMinInput)
{
  // 入力値の絶対値をチェックしてデッドゾーン以下は0に
  const double radiusSquared = FMath::Square(InX) + FMath::Square(InY);
  const double moveDeadZoneSquared = FMath::Square(FMath::Max(0.0, InDeadZone));

  // デッドゾーン以下の入力を受け付けない
  if ((radiusSquared <= moveDeadZoneSquared) || FMath::IsNearlyZero(radiusSquared))
  {
    return;
  }

  // Input max value(scalar)
  constexpr double MAX_INPUT_VALUE = 1.0;
  
  const double realMinInput = FMath::Min(InMinInput, MAX_INPUT_VALUE);
  // インプット閾値レベル
  const TArray<double> inputThresholdLevel{ 
                        realMinInput,       // LV1  : 最小入力値
                        MAX_INPUT_VALUE,    // LVMax: 最大入力値
                      };

  double adjustedForward = InY;
  double adjustedRight = InX;
  for (int32 idx = 0; idx < inputThresholdLevel.Num(); ++idx)
  {
    // インプット閾値まで補正する
    const double inputLevelValue = inputThresholdLevel[idx];
    const double inputLevelValueSquared = FMath::Square(inputLevelValue);
    if (radiusSquared < inputLevelValueSquared)
    {
      const double inputModifier = inputLevelValue / FMath::Sqrt(radiusSquared);
      adjustedForward *= inputModifier;
      adjustedRight   *= inputModifier;
      break;
    }   
  }

  if (ViewCharacter != nullptr)
  {
    ViewCharacter->DoMove(adjustedRight, adjustedForward);
  }
}

void UARPlayerPresenter::HandleSnapTargetUpdate(double InX, double InY)
{
  const bool bIsInputValid = !FMath::IsNearlyZero(InX) && !FMath::IsNearlyZero(InY);
  if (!bIsInputValid)
  {
    Model.TargetSnapInputDirection = FVector2D::ZeroVector;
    Model.bIsReadyToSearchSnapTarget = false;
    return;
  }

  if (Model.bCanUpdateSnapMovement)
  {
    return;
  }

  Model.bIsReadyToSearchSnapTarget = true;
  Model.bCanUpdateSnapMovement = false;
  Model.TargetSnapInputDirection.X = InX;
  Model.TargetSnapInputDirection.Y = InY;
}

void UARPlayerPresenter::RegisterSnapTargetTask()
{
  if (ViewCharacter == nullptr)
  {
    return;
  }

  Model.SnapTargetActor.Reset();
  Model.TargetSnapInputDirection.Normalize();

  const FVector curtPlayerDir = ViewCharacter->GetActorForwardVector();
  const FRotator curtCtrlRotation = ViewCharacter->GetController()->GetControlRotation();
  const FRotator yawRotation{0.0, curtCtrlRotation.Yaw, 0.0};
  const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
  const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
  const FVector targetPlayerDir = ((forwardDirection * Model.TargetSnapInputDirection.Y) + (rightDirection * Model.TargetSnapInputDirection.X)).GetSafeNormal();
  const FVector startLoc = ViewCharacter->GetActorLocation();
  const FVector endLoc = startLoc + targetPlayerDir * Model.TargetSnapDetectMaxRange;
  const float radius = ViewCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

  // TODO Range Detectorなど既存の機能を使う
  TArray< TEnumAsByte<EObjectTypeQuery> > objTypes{}; 
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
  TArray<AActor*> ignoreActors{};
  ignoreActors.Add(ViewCharacter);
  TArray<FHitResult> outResults{};

  const bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
                                            ViewCharacter,
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

      // プレイヤーを始点とした検索範囲のうち、プレイヤーの後ろの半円に入っているターゲットを無視する
      const FVector dirToTarget = (hitActor->GetActorLocation() - ViewCharacter->GetActorLocation()).GetSafeNormal();
      if (FVector::DotProduct(dirToTarget, targetPlayerDir) < 0.0f)
      {
        continue;
      }

      // 攻撃できないターゲットに吸着しない
      if (IARAttackable::IsActorAttackable(hitActor))
      {
        const float curtHitResultDistanceSquared = (playerLoc - hitResult.GetActor()->GetActorLocation()).SquaredLength();
        if (Model.SnapTargetActor.IsValid())
        {
          const float curtMinDistanceSquared = (playerLoc - Model.SnapTargetActor->GetActorLocation()).SquaredLength();
          // 前の吸着対象より近い対象に更新
          if (curtMinDistanceSquared > curtHitResultDistanceSquared)
          {
            UpdateSnapTargetImpactPoint(hitResult.GetComponent(), hitResult.ImpactPoint);
          }   
        }
        else
        {
          UpdateSnapTargetImpactPoint(hitResult.GetComponent(), hitResult.ImpactPoint);
        }

        Model.bCanUpdateSnapMovement = true;
      }
    }

    if (Model.bCanUpdateSnapMovement)
    {
      Model.SnapStartPosition = ViewCharacter->GetActorLocation();
      Model.SnapStartRotation = ViewCharacter->GetActorRotation();
      Model.bIsReadyToSearchSnapTarget = false;

      // 吸着移動処理をキャラクターのTick処理に追加
      Handle_UpdateSnapTarget = ViewCharacter->TickTaskDelegate.AddUObject(this, &ThisClass::UpdateSnapTarget);
    }
  }
}

void UARPlayerPresenter::UpdateSnapTarget(float DeltaTime)
{
  if (ViewCharacter == nullptr)
  {
    return;
  }

  FVector newLocation = ViewCharacter->GetActorLocation();
  FRotator newRotation = ViewCharacter->GetActorRotation();

  if ((Model.SnapTargetActor.IsValid()) && 
      !FMath::IsNearlyZero(Model.SnapTimeInterval) && 
      (Model.SnapTimeCounter < Model.SnapTimeInterval)
    )
  {
    const float alphaMin = 0.0f;
    const float alphaMax = 1.0f;

    Model.SnapTimeCounter += DeltaTime;

    // 吸着先に向ける回転を計算する
    {
      const float RotateInterpInterval = Model.SnapTimeInterval / 2.0f;
      FVector faceToTarget_XYAxis = (Model.SnapTargetActor->GetActorLocation() - ViewCharacter->GetActorLocation());
      faceToTarget_XYAxis.Z = 0.0;
      const FRotator faceToTargetRot = faceToTarget_XYAxis.Rotation();
      const float rotLerpAlpha = FMath::Clamp((Model.SnapTimeCounter / RotateInterpInterval), alphaMin, alphaMax);
      newRotation = FMath::InterpCircularOut(Model.SnapStartRotation, faceToTargetRot, rotLerpAlpha);
    }
    
    // 吸着先に向ける座標を計算する
    {
      const FVector playerToTargetOffset = Model.SnapTargetImpactPoint.GetSafeNormal() * ViewCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
      const FVector newTargetLocation = Model.SnapTargetActor->GetActorLocation() + Model.SnapTargetImpactPoint + playerToTargetOffset;
      const FVector newTargetLocation_UsePlayerZ = FVector{newTargetLocation.X, newTargetLocation.Y, ViewCharacter->GetActorLocation().Z};
      const float locLerpAlpha = FMath::Clamp((Model.SnapTimeCounter / Model.SnapTimeInterval), alphaMin, alphaMax);
      newLocation = FMath::InterpCircularIn(Model.SnapStartPosition, newTargetLocation_UsePlayerZ, locLerpAlpha);
    }
  }
  else
  {
    StopSnapTargetInternal();
  }

  ViewCharacter->OnTargetSnapped(newLocation, newRotation);
}

void UARPlayerPresenter::UnregisterSnapTargetTask()
{
  StopSnapTargetInternal();

  Model.bIsInComboAction = false;

  if (ViewCharacter != nullptr)
  {
    ViewCharacter->TickTaskDelegate.Remove(Handle_UpdateSnapTarget);
    Handle_UpdateSnapTarget.Reset();
  }
}

void UARPlayerPresenter::HandleCharacterChargeRotate(double InX, double InY)
{
  // 使わないため
  (void)InY;

  const bool bCanRotate = (ViewCharacter != nullptr) && !Model.bIsInAir && Model.bIsCharging;
  if (!bCanRotate)
  {
    return;
  }

  // 回転範囲を絞る
  const FRotator curtPlayerDir_Rot = ViewCharacter->GetActorRotation();
  const double curtYawOffsetToChargeStart = curtPlayerDir_Rot.Yaw - Model.ChargeStartFaceDir.Rotation().Yaw;
  double rotate_Yaw = InX;
  if (FMath::Abs(curtYawOffsetToChargeStart + rotate_Yaw) > Model.ChargeRotateHalfRange)
  {
    rotate_Yaw = FMath::Sign(rotate_Yaw) * (Model.ChargeRotateHalfRange - FMath::Abs(curtYawOffsetToChargeStart));
  }
  
  ViewCharacter->DoRotate(rotate_Yaw);
}

void UARPlayerPresenter::HandleBattleResult(AARRangerCharacter* InAffectedCharacter, const ARRanger::Battle::FARDamageResult& InDamageResult)
{
  check(InAffectedCharacter == ViewCharacter);

  // ダメージが正の数なので、HPの変化量を負の数にする
  const float HPChangeValue = -InDamageResult.FinalDamage;
  bool bIsDead = false;
  if (Model.HealthComponent != nullptr)
  {
    Model.HealthComponent->HandleHealthChange(InDamageResult.Instigator, HPChangeValue);

    // ノックバック処理
    if (Model.HealthComponent->GetHealth() > 0.0f)
    {
      constexpr double launchPowerFactor = 1.0;

      FVector launchDirNorm = InDamageResult.FinalLaunchDirection;
      launchDirNorm.Z = 0.0;
      launchDirNorm.Normalize();
      if (ViewCharacter != nullptr)
      {
        const double finalLaunchPower = Model.LaunchPower * launchPowerFactor;
        ViewCharacter->LaunchCharacter_Ext(launchDirNorm, finalLaunchPower);
      }
    }
    else
    {
      bIsDead = true;
    }
  }

  if (ViewCharacter != nullptr)
  {
    ViewCharacter->OnHealthChanged(InDamageResult.Instigator, HPChangeValue, bIsDead);
  }

}

void UARPlayerPresenter::HandleBattleStateChange(bool bIsInBattle)
{
  // 戦闘中だと自動回復を止める
  if (Model.HealthComponent != nullptr)
  {
    Model.HealthComponent->SetAutoRegenerationEnable(!bIsInBattle);
  }
}

void UARPlayerPresenter::HandleTransformedEvent(EARMagnetismType InNewTransformation)
{
  if (ViewCharacter == nullptr)
  {
    return;
  }

  // 変身した瞬間、一回だけ周りに磁力オブジェクトあるかを確認する、あったらタスクを登録する
  switch (InNewTransformation)
  {
    // 引力状態に変身
    case EARMagnetismType::Attraction:
    {
      // Try to climb
      TArray<UPrimitiveComponent*> OverlappingComps{};
      ViewCharacter->GetOverlappingComponents(OverlappingComps);

      for (UPrimitiveComponent* Comp : OverlappingComps)
      {
        if (Comp->GetOwner()->IsA<AInsekiClimbingObject>())
        {
          StartClimbing();
          break;
        }
      }
    }
    break;

    // 斥力状態に変身 
    case EARMagnetismType::Repulsion:
    {
      const UWorld* world = GetWorld();
      UCapsuleComponent* capsuleComp = ViewCharacter->GetCapsuleComponent();
      
      // Use player character CapsuleComponent as collision shape.Make it a bit larger than origin shape
      const float extendCapsuleRadius = 5.f;
      const float extendCapsuleHalfHeight = 5.f;
      const float shapeRadius = capsuleComp->GetScaledCapsuleRadius() + extendCapsuleRadius;
      const float shapeHalfHeight = capsuleComp->GetScaledCapsuleHalfHeight() + extendCapsuleHalfHeight;
      const FCollisionShape characterCapsuleShape = FCollisionShape::MakeCapsule(shapeRadius, shapeHalfHeight);
      const FVector origin = capsuleComp->GetComponentLocation();
      const FQuat originQuat = capsuleComp->GetComponentQuat();

      // TODO Use RangeDetector
      FCollisionObjectQueryParams objQueryParams{};
      objQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
      objQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
      objQueryParams.AddObjectTypesToQuery(ECC_Pawn);
      objQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

      FCollisionQueryParams queryParams{SCENE_QUERY_STAT(HandleTransformedEvent), false, ViewCharacter};
      queryParams.bReturnPhysicalMaterial = true;

      TArray<FHitResult> outHitResult{};
      const bool bHit = world->SweepMultiByObjectType(
                                                      outHitResult, 
                                                      origin, 
                                                      origin,
                                                      originQuat,
                                                      objQueryParams,
                                                      characterCapsuleShape,
                                                      queryParams);

      if (bHit)
      {
        for (const FHitResult& hit : outHitResult)
        {
          AActor* hitActor = hit.GetActor();
          if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(hitActor))
          {
            Physics_RegisterMagneticTask_Once(ViewCharacter, magnetizableObj);
          }
        }
      }
    }
    break;
  }
}

void UARPlayerPresenter::HandleAttackAbilityStarted()
{
  if (Model.bIsReadyToSearchSnapTarget && !Model.TargetSnapInputDirection.IsNearlyZero())
  {
    RegisterSnapTargetTask();
  }

  Model.bIsInComboAction = true;
}

void UARPlayerPresenter::HandleAttackAbilityEnded()
{
  UnregisterSnapTargetTask();
}

void UARPlayerPresenter::StartClimbing()
{
  const bool bCanStartClimb = !Model.bIsClimbing 
                              && (ViewCharacter != nullptr)
                              && (ViewCharacter->GetMagnetismType() == EARMagnetismType::Attraction);
  if (!bCanStartClimb)
  {
    return;
  }

  Model.bIsClimbing = true;
  ViewCharacter->OnClimbStarted();
  Handle_UpdateClimbing = ViewCharacter->TickTaskDelegate.AddUObject(this, &ThisClass::UpdateClimbing);

  // 壁があるかを判定
  constexpr float wallCheckRayLength = 100.0f;
  const FVector Start = ViewCharacter->GetActorLocation();
  const FVector End = Start + ViewCharacter->GetActorForwardVector() * wallCheckRayLength;

  FHitResult HitResult{};
  FCollisionQueryParams Params{};
  Params.AddIgnoredActor(ViewCharacter);
  const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
  if (!bHit)
  {
    StopClimbing();
    return;
  }

  // 壁の法線を保存
  Model.ClimbSurfaceNormal = HitResult.ImpactNormal;
}

void UARPlayerPresenter::StopClimbing()
{
  if ((ViewCharacter == nullptr) || !Model.bIsClimbing)
  {
    return;
  }

  Model.bIsClimbing = false;
  Model.ClimbSurfaceNormal = FVector::ZeroVector;

  ViewCharacter->OnClimbEnded();
  ViewCharacter->TickTaskDelegate.Remove(Handle_UpdateClimbing);
  Handle_UpdateClimbing.Reset();
}

void UARPlayerPresenter::UpdateClimbing(float DeltaTime)
{
  if (!CanUpdateClimbingInternal())
  {
    StopClimbing();
    return;
  }
  
  constexpr float climbSpeed = 2100.0f; // 上昇速度
  const FVector climbMovement{0.0, 0.0, climbSpeed * DeltaTime};
  ViewCharacter->OnClimbUpdated(climbMovement);
}

void UARPlayerPresenter::UpdateSnapTargetImpactPoint(const USceneComponent* InOverlappedComp, const FVector InImpactPoint)
{
  check(InOverlappedComp != nullptr);

  Model.SnapTargetActor = InOverlappedComp->GetOwner();
  const FVector relativeImpactPoint = InImpactPoint - InOverlappedComp->GetComponentLocation();
  // We do not use Z-component of impact point
  Model.SnapTargetImpactPoint = FVector{relativeImpactPoint.X, relativeImpactPoint.Y, 0.0};
}

void UARPlayerPresenter::OnGroundLanded(const FHitResult& InHit)
{
  Model.bIsInAir = false;

  if (ViewCharacter != nullptr)
  {
    ViewCharacter->OnLanded(InHit);
  }
}

void UARPlayerPresenter::OnClimbSurfaceOverlapBegan(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (OtherActor == nullptr)
  {
    return;
  }

  if (OtherActor->IsA<AInsekiClimbingObject>())
  {
    StartClimbing();
  }
}

void UARPlayerPresenter::OnClimbSurfaceOverlapEnded(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  if (OtherActor == nullptr)
  {
    return;
  }

  if (OtherActor->IsA<AInsekiClimbingObject>())
  {
    StopClimbing();
  }
}


void UARPlayerPresenter::OnMagneticForceFieldBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (ViewCharacter != nullptr)
  {
    if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
    {
      Physics_RegisterMagneticTask(ViewCharacter, magnetizableObj);
    }
  }
}

void UARPlayerPresenter::OnMagneticForceFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
  if (ViewCharacter != nullptr)
  {
    if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
    {
      Physics_UnregisterMagneticTask(ViewCharacter, magnetizableObj);
    }
  }
}

void UARPlayerPresenter::OnMagnetizedObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
  if (ViewCharacter != nullptr)
  {
    // 斥力状態だけHit判定する
    if (ViewCharacter->GetMagnetismType() != EARMagnetismType::Repulsion)
    {
      return;
    }

    if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
    {
      Physics_RegisterMagneticTask_Once(ViewCharacter, magnetizableObj);
    }
  }
}

void UARPlayerPresenter::OnCharacterJumpStarted()
{
  if (Model.bIsClimbing)
  {
    StopClimbing();
  }

  Model.bIsInAir = true;

}

void UARPlayerPresenter::OnCharacterJumpStopped()
{
  // TODO Empty implementation
}

bool UARPlayerPresenter::CanUpdateClimbingInternal() const
{
  const bool bCanUpdateClimbing = Model.bIsClimbing
                                && (ViewCharacter != nullptr);
  if (!bCanUpdateClimbing)
  {
    return false;
  }

  // 壁回転処理
  // 足元の位置（Capsuleの底の位置）
  const UCapsuleComponent* capsuleComp = ViewCharacter->GetCapsuleComponent();
  const float capsuleHalfHeight = capsuleComp->GetScaledCapsuleHalfHeight();
  const FVector curtLocation = ViewCharacter->GetActorLocation();
  // 壁に対して垂直な向きに少しめり込むようにして設定
  // FIXME Remove magic number
  const FVector Start = curtLocation - Model.ClimbSurfaceNormal * (capsuleHalfHeight - 5.0f);
  const FVector End = Start - Model.ClimbSurfaceNormal * 7.0f;

  FHitResult HitResult{};
  FCollisionQueryParams Params{};
  Params.AddIgnoredActor(ViewCharacter);

  const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
  const bool bCanKeepClimbingState = bHit && ViewCharacter->GetMagnetismType() == EARMagnetismType::Attraction;

  return bCanKeepClimbingState;
}

void UARPlayerPresenter::StopSnapTargetInternal()
{
  Model.bIsReadyToSearchSnapTarget = false;
  Model.bCanUpdateSnapMovement = false;
  Model.TargetSnapInputDirection = FVector2D::ZeroVector;
  Model.SnapTimeCounter = 0.0f;
  Model.SnapTargetActor.Reset();
}

void UARPlayerPresenter::OnCameraRigChanged(ECameraRigType OldType, ECameraRigType NewType)
{
  if (Model.CameraRouter != nullptr)
  {
    Model.CameraRouter->ChangeCameraRig(NewType);
  }
}

void UARPlayerPresenter::OnLockOnDataUpdated(const FVector& CameraPos, const FRotator& CameraRot, const TArray<AActor*>& Targets)
{
  const bool bCanUpdateTarget = true;
  if (bCanUpdateTarget)
  {
    TArray<AActor*> targetsInCameraView{};
    // ロックオンできるターゲットを絞る
    FilterTargetsInCamera(CameraPos, CameraRot, Targets, targetsInCameraView);
    Model.UpdateLockOnTargets(ViewCharacter, targetsInCameraView);
  }
}

void UARPlayerPresenter::OnLockOnTargetUpdated(AActor* TargetActor)
{
  if (TargetActor != nullptr)
  {
    if (GEngine)
    {
      GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("OnLockOnTargetUpdated: %s"), *TargetActor->GetName()));
    }
  }

  // FIXME Give the target to view(Update UI and camera);
}

void UARPlayerPresenter::FilterTargetsInCamera(const FVector& CameraPos, const FRotator& CameraRot, const TArray<AActor*>& OriginTargets, TArray<AActor*>& OutTargets)
{
  OutTargets.Reset();
  for (AActor* target : OriginTargets)
  {
    // カメラに入っているかつプレイヤーの前にあるActor(カメラとプレイヤーの間は対象外)だけ残す
    if (IsActorInCameraView(target) && IsActorInFrontOfPlayer(CameraPos, target))
    {
      OutTargets.Emplace(target);
    }
  }
}

bool UARPlayerPresenter::IsActorInCameraView(AActor* TargetActor) const
{
  if ((TargetActor == nullptr) || !m_controller.IsValid())
  {
    return false;
  }

  // ビューポートにあるActorを取る
  FVector2D screenLoc{};
  bool bProjected = m_controller->ProjectWorldLocationToScreen
                        (
                          TargetActor->GetActorLocation(),
                          screenLoc,
                          true        // bPlayerViewportRelative
                        );
  if (!bProjected)
  {
    return false;
  }

  int32 viewportSizeX{0}, viewportSizeY{0};
  m_controller->GetViewportSize(viewportSizeX, viewportSizeY);

  return    (screenLoc.X >= 0.0 && screenLoc.X <= (double)viewportSizeX)
         && (screenLoc.Y >= 0.0 && screenLoc.Y <= (double)viewportSizeY);
}

bool UARPlayerPresenter::IsActorInFrontOfPlayer(const FVector& CameraPos, AActor* TargetActor) const
{
  if ((ViewCharacter == nullptr) || (TargetActor == nullptr))
  {
    return false;
  }

  // 俯瞰視点で、カメラとプレイヤーの間にあるActorを絞る
  FVector cameraToPlayer = ViewCharacter->GetActorLocation() - CameraPos;
  FVector cameraToTarget = TargetActor->GetActorLocation() - CameraPos;
  // Z座標が必要ないため、0に設定 
  cameraToPlayer.Z = 0.0;
  cameraToTarget.Z = 0.0;

  // 後ろにあるActorを除く（直角方向も）
  const double dotProduct = FVector::DotProduct(cameraToPlayer.GetSafeNormal(), cameraToTarget.GetSafeNormal());
  if (dotProduct <= 0)
  {
    return false;
  }

  // カメラからプレイヤーへの方向ベクトル上、距離が小さいActor（カメラとプレイヤーの間にある）を除く
  if (cameraToTarget.SquaredLength() * dotProduct <= cameraToPlayer.SquaredLength())
  {
    return false;
  }

  return true;
}

namespace 
{
  double CalcLockTargetPriority(const FVector& TargetDirNorm, const FVector& BaseDirNorm)
  {
    const FVector Cross = FVector::CrossProduct(TargetDirNorm, BaseDirNorm);
    const double SignedDotVal = FVector::DotProduct(TargetDirNorm, BaseDirNorm) * FMath::Sign(Cross.Z);

    return (SignedDotVal < 0.0) ? -SignedDotVal - 1 : SignedDotVal;
  }
}
