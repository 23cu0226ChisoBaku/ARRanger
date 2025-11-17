// Fill out your copyright notice in the Description page of Project Settings.

#include "Presentation/ARPlayerPresenter.h"

#include "Character/ARRangerCharacter.h"
#include "Character/ARHealthComponent.h"
#include "BattleSystem/IARAttackable.h"
#include "Components/CapsuleComponent.h"

#include "Magnetic/InsekiClimbingObject.h"

#include "Kismet/KismetSystemLibrary.h"

namespace
{
  // Input max value(scalar)
  constexpr double MAX_INPUT_VALUE = 1.0;
}

FARPlayerModel::FARPlayerModel()
  : HealthComponent{nullptr}
  , ChargeStartFaceDir{EForceInit::ForceInitToZero}
  , ClimbSurfaceNormal{EForceInit::ForceInitToZero}
  , TargetSnapInputDirection{EForceInit::ForceInitToZero}
  , LaunchPower{400.0}
  , ChargeRotateHalfRange{60.0}
  , TargetSnapDetectMaxRange{200.0}
  , SnapTimeInterval{0.2f}
  , SnapTimeCounter{0.0f}
  , SnapTargetActor{nullptr}
  , bIsCharging{false}
  , bIsInAir{false}
  , bIsClimbing{false}
  , bIsInComboAction{false}
  , bCanUpdateSnapMovement{false}
  , bIsReadyToSearchSnapTarget{false}
{ }

void FARPlayerModel::Initialize(AARRangerCharacter* InViewCharacter)
{
  check(InViewCharacter != nullptr);

  HealthComponent = static_cast<UARHealthComponent*>(InViewCharacter->GetComponentByClass(UARHealthComponent::StaticClass()));
}

void FARPlayerModel::Reset()
{
  HealthComponent = nullptr;
}

void UARPlayerPresenter::Initialize(AARRangerCharacter* InViewCharacter)
{
  check(InViewCharacter != nullptr);

  if (ViewCharacter == InViewCharacter)
  {
    return;
  }

  if (ViewCharacter != nullptr)
  {
    Deinitialize();
  }

  ViewCharacter = InViewCharacter;
  if (ViewCharacter != nullptr)
  {
    ViewCharacter->OnBattleResultAccepted.AddUObject(this, &ThisClass::HandleBattleResult);
    ViewCharacter->OnBattleStateChanged.AddUObject(this, &ThisClass::HandleBattleStateChange);
    ViewCharacter->OnTransformed.AddUObject(this, &ThisClass::HandleTransformedEvent);
    ViewCharacter->OnJumpedDelegate.AddUObject(this, &ThisClass::OnCharacterJumpStarted);
    ViewCharacter->OnJumpStoppedDelegate.AddUObject(this, &ThisClass::OnCharacterJumpStopped);
    ViewCharacter->CameraRigChangeEvent.AddUObject(this, &ThisClass::OnCameraRigChanged);
    ViewCharacter->AttackAbilityStartDelegate.AddUObject(this, &ThisClass::HandleAttackAbilityStarted);
    ViewCharacter->AttackAbilityEndDelegate.AddUObject(this, &ThisClass::HandleAttackAbilityEnded);

    // ACharacter delegate
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
  // Unbind to ACharacter delegate
  ViewCharacter->LandedDelegate.RemoveDynamic(this, &ThisClass::OnGroundLanded);

  ViewCharacter->CameraRigChangeEvent.RemoveAll(this);

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
  const bool bInputAllowed = (ViewCharacter != nullptr) 
                             && !Model.bIsClimbing;
  if (!bInputAllowed)
  {
    return;
  }

  // TODO Maybe use state pattern here
  if (Model.bIsCharging)
  {
    HandleCharacterChargeRotate(InX, InY);
  }
  else if (Model.bIsInComboAction)
  {
    HandleSnapTargetUpdate(InX, InY);
  }
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

  const bool bCanTransform = true;
  if (!bCanTransform)
  {
    return;
  }

  ViewCharacter->Transform();
}

void UARPlayerPresenter::Input_HandleCameraReset()
{
  if (ViewCharacter == nullptr)
  {
    return;
  }

  const bool bCanResetCamera = true;
  if (!bCanResetCamera)
  {
    return;
  }

  ViewCharacter->SetCameraRig(ECameraRigType::Reset);
}

void UARPlayerPresenter::HandleChargeStart()
{
  Model.bIsCharging = true;
  if (ViewCharacter != nullptr)
  {
    Model.ChargeStartFaceDir = ViewCharacter->GetActorForwardVector();
  }
}

void UARPlayerPresenter::HandleChargeEnd()
{
  Model.bIsCharging = false;
}

void UARPlayerPresenter::HandleCharacterMove(double InX, double InY, double InDeadZone, double InMinInput)
{
  // 入力値の絶対値をチェックしてデッドゾーン以下は0に
  const double radiusSquared = FMath::Square(InX) + FMath::Square(InY);
  const double moveDeadZoneSquared = FMath::Square(FMath::Max(0.0, InDeadZone));

  // デッドゾーン以下
  if (radiusSquared <= moveDeadZoneSquared)
  {
    return;
  }
  
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
    // Move character
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

  // NOTE Magic number is bad
  float radius = 200.f;
  if (UCapsuleComponent* capsule = ViewCharacter->GetCapsuleComponent())
  {
    radius = capsule->GetScaledCapsuleHalfHeight();
  }

  // TODO use RangeDetector
  TArray< TEnumAsByte<EObjectTypeQuery> > objTypes{}; 
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
  objTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
  TArray<AActor*> ignoreActors{};
  ignoreActors.Add(ViewCharacter);

  TArray<FHitResult> outResults{};

  // TODO Range Detectorなど既存の機能を使う
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
      const FVector dirToTarget = (hitActor->GetActorLocation() - ViewCharacter->GetActorLocation()).GetSafeNormal();
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
        if (Model.SnapTargetActor.IsValid())
        {
          // Find min distance to player
          const float curtMinDistanceSquared = (playerLoc - Model.SnapTargetActor->GetActorLocation()).SquaredLength();
          if (curtMinDistanceSquared > curtHitResultDistanceSquared)
          {
            Model.SnapTargetActor = hitResult.GetActor();
            UPrimitiveComponent* targetPrimitiveComp = hitResult.GetComponent();
            const FVector relativeImpactPoint = hitResult.ImpactPoint - targetPrimitiveComp->GetComponentLocation();
            // We do not use Z-component of impact point
            Model.SnapTargetImpactPoint = FVector{relativeImpactPoint.X, relativeImpactPoint.Y, 0.0};
          }   
        }
        else
        {
          Model.SnapTargetActor = hitResult.GetActor();
          UPrimitiveComponent* targetPrimitiveComp = hitResult.GetComponent();
          const FVector relativeImpactPoint = hitResult.ImpactPoint - targetPrimitiveComp->GetComponentLocation();
          // We do not use Z-component of impact point
          Model.SnapTargetImpactPoint = FVector{relativeImpactPoint.X, relativeImpactPoint.Y, 0.0};
        }

        Model.bCanUpdateSnapMovement = true;
      }
    }

    if (Model.bCanUpdateSnapMovement)
    {
      Model.SnapStartPosition = ViewCharacter->GetActorLocation();
      Model.SnapStartRotation = ViewCharacter->GetActorRotation();
      Model.bIsReadyToSearchSnapTarget = false;

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

    // Calculate new rotation
    const float RotateInterpInterval = Model.SnapTimeInterval / 2.0f;
    FVector faceToTarget_XYAxis = (Model.SnapTargetActor->GetActorLocation() - ViewCharacter->GetActorLocation());
    faceToTarget_XYAxis.Z = 0.0;
    const FRotator faceToTargetRot = faceToTarget_XYAxis.Rotation();
    const float rotLerpAlpha = FMath::Clamp((Model.SnapTimeCounter / RotateInterpInterval), alphaMin, alphaMax);
    newRotation = FMath::InterpCircularOut(Model.SnapStartRotation, faceToTargetRot, rotLerpAlpha);
    
    // Calculate new location 
    const FVector playerToTargetOffset = Model.SnapTargetImpactPoint.GetSafeNormal() * ViewCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const FVector newTargetLocation = Model.SnapTargetActor->GetActorLocation() + Model.SnapTargetImpactPoint + playerToTargetOffset;
    const FVector newTargetLocation_UsePlayerZ = FVector{newTargetLocation.X, newTargetLocation.Y, ViewCharacter->GetActorLocation().Z};
    const float locLerpAlpha = FMath::Clamp((Model.SnapTimeCounter / Model.SnapTimeInterval), alphaMin, alphaMax);
    newLocation = FMath::InterpCircularIn(Model.SnapStartPosition, newTargetLocation_UsePlayerZ, locLerpAlpha);
  }
  // Stop snapping
  else
  {
    StopSnapTargetInternal();
  }

  ViewCharacter->OnTargetSnapped(newLocation, newRotation);

}

void UARPlayerPresenter::UnregisterSnapTargetTask()
{
  StopSnapTargetInternal();

  // TODO
  Model.bIsInComboAction = false;

  if (ViewCharacter != nullptr)
  {
    ViewCharacter->TickTaskDelegate.Remove(Handle_UpdateSnapTarget);
    Handle_UpdateSnapTarget.Reset();
  }
}

void UARPlayerPresenter::HandleCharacterChargeRotate(double InX, double InY)
{
  // 使わない
  (void)InY;

  const bool bCanRotate = (ViewCharacter != nullptr) && !Model.bIsInAir && Model.bIsCharging;
  if (!bCanRotate)
  {
    return;
  }

  const FRotator curtPlayerDir_Rot = ViewCharacter->GetActorRotation();
  const double curtYawOffsetToChargeStart = curtPlayerDir_Rot.Yaw - Model.ChargeStartFaceDir.Rotation().Yaw;

  double rotate_Yaw = InX;
  if (FMath::Abs(curtYawOffsetToChargeStart + rotate_Yaw) > Model.ChargeRotateHalfRange)
  {
    rotate_Yaw = FMath::Sign(rotate_Yaw) * (Model.ChargeRotateHalfRange - FMath::Abs(curtYawOffsetToChargeStart));
  }
  
  // Rotate Character
  ViewCharacter->DoRotate(rotate_Yaw);
  
}

void UARPlayerPresenter::HandleBattleResult(AARRangerCharacter* InAffectedCharacter, const ARRanger::Battle::FARDamageResult& InDamageResult)
{
  // check AffectedCharacter is same as View character
  check(InAffectedCharacter == ViewCharacter);

  // Value of damage is positive. Make it negative
  const float HPChangeValue = -InDamageResult.FinalDamage;
  bool bIsDead = false;

  if (Model.HealthComponent != nullptr)
  {
    Model.HealthComponent->HandleHealthChange(InDamageResult.Instigator, HPChangeValue);

    if (Model.HealthComponent->GetHealth() > 0.0f)
    {
      FVector launchDirNorm = InDamageResult.FinalLaunchDirection;
      launchDirNorm.Z = 0.0;
      launchDirNorm.Normalize();
      const double launchPowerFactor = 1.0;

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
  // TODO We should not let auto generation system in HealthComponent
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

  switch (InNewTransformation)
  {
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

    // Try triggering physics event 
    case EARMagnetismType::Repulsion:
    {
      const UWorld* world = GetWorld();
      UCapsuleComponent* capsuleComp = ViewCharacter->GetCapsuleComponent();
      
      // Use player character CapsuleComponent as collision shape.Make it a bit larger than origin shape
      // TODO Maybe we can remove this magic number?
      const float extendCapsuleRadius = 5.f;
      const float extendCapsuleHalfHeight = 5.f;
      const float shapeRadius = capsuleComp->GetScaledCapsuleRadius() + extendCapsuleRadius;
      const float shapeHalfHeight = capsuleComp->GetScaledCapsuleHalfHeight() + extendCapsuleHalfHeight;
      const FCollisionShape characterCapsuleShape = FCollisionShape::MakeCapsule(shapeRadius, shapeHalfHeight);
      const FVector origin = capsuleComp->GetComponentLocation();
      const FQuat originQuat = capsuleComp->GetComponentQuat();

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

  // TODO
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
  const FVector Start = ViewCharacter->GetActorLocation();
  // FIXME Magic number is bad
  const FVector End = Start + ViewCharacter->GetActorForwardVector() * 100.0f;

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

  // NOTE Hard coding magic number is bad
  const float climbSpeed = 2100.0f; // 上昇速度
  const FVector climbMovement{0.0, 0.0, climbSpeed * DeltaTime};
  ViewCharacter->OnClimbUpdated(climbMovement);
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
    if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
    {
      Physics_RegisterMagneticTask_Once(ViewCharacter, magnetizableObj);
    }
  }
}

void UARPlayerPresenter::OnCameraRigChanged(ECameraRigType InType)
{
  // TODO Empty implementation
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
  // TODO
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