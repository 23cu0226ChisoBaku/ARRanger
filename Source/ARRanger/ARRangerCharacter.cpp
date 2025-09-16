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
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
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

#include "Pawn/ARPawnInitComponent.h"

#include "MLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

namespace
{
  static const float MAX_INPUT_VALUE = 1.0f;
}

AARRangerCharacter::AARRangerCharacter()
	: currentClimbSurface(nullptr)
	, bIsClimbed(false)
	, Montage_AttractionClimb(nullptr)
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
    if (TargetToSnap != nullptr)
    {
      // TODO
      SetActorLocation(TargetToSnap->GetActorLocation());
    }

    bCanTargetSnap = false;
  }

	bool isLockedOn = LockOnComponent->GetIsLockedOn();
	AActor* Target = LockOnComponent->GetLockedOnTarget();

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

	// 引力クライム中に処理
	if (bIsClimbed)
	{
		const float ClimbSpeed = 700.0f; // 上昇速度
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
			if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
			{
				MyAnim->IsClimbing = false;
			}

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

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	
	if (bHit)
	{
		//// 壁に対して垂直になるようキャラを回転させる
		//const FVector X = GetActorUpVector().GetSafeNormal();
		//const FVector Z = HitResult.Normal.GetSafeNormal();
		//const FRotator NewRot = FRotationMatrix::MakeFromXZ(X, Z).Rotator();
		//SetActorRotation(NewRot);

		// 壁の法線を保存
		wallNormal = HitResult.ImpactNormal;

		// UpをwallNormalにする
		FVector Up = wallNormal;

		// 前方向を作成（Up とワールド右ベクトルから計算）
		FVector Forward = FVector::CrossProduct(Up, FVector::RightVector).GetSafeNormal();

		// 回転を作成
		const FRotator NewRot = FRotationMatrix::MakeFromXZ(Forward, Up).Rotator();
		SetActorRotation(NewRot);
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

	// ジャンプ処理
	Jump();
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
	// if (GA_PunchInstance)
	// {
	// 	GA_PunchInstance->StartPunch();
	// }
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

	// 現在と別のモードに変身
	SetMagnetismType(
		(GetMagnetismType() == EARMagnetismType::Attraction)
		? EARMagnetismType::Repulsion
		: EARMagnetismType::Attraction);

	// メッシュを別モードに変更
	USkeletalMesh* NewMesh = (GetMagnetismType() == EARMagnetismType::Repulsion)
		? RepulsionMesh
		: AttractionMesh;

	if (NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
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
		attractSpecialAttackComponent->OnStartSpecialAttract();
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
}



#pragma endregion IARAttackerInterface implementation
/**End IARAttackerInterface implementation */

// TODO Temporary blueprint callable function 
void AARRangerCharacter::OnPunchStarted()
{
  // TODO
  if (bReadyToTargetSnap && !TargetSnapInput.IsNearlyZero())
  {
    bCanTargetSnap = true;
    SearchTargetToSnap();
  }

  bReadyToTargetSnap = false;
  
  if (AttackBaseComp != nullptr)
  {
    AttackBaseComp->SetIsAttacked(false);
    AttackBaseComp->SetIsStrongAttacked(false);
    AttackBaseComp->SetIsAttractingEnemy(false);

    AttackBaseComp->RotateOwnerToTarget();

    if (GetIsApproachedEnemy())
    {
      UE_LOG(LogTemp, Warning, TEXT("AttractionPunch"))
      AttackBaseComp->SetIsAttacked(true);
      SetIsAttracted(false);
      SetIsApproachedEnemy(false);

      return;
    }

    if (GetMagnetismType() == EARMagnetismType::Attraction && GetIsLockedOn() && !GetIsAttracted())
    {
      // TODO
    }

    if (bIsInComboWindow)
    {
      const int32 MaxCombo = 3;
      if (GetComboCount() < MaxCombo - 1)
      {

      }
    }
  }
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
    return;
  }

  bReadyToTargetSnap = true;
  bCanTargetSnap = false;
  TargetSnapInput = InputDir;
}

void AARRangerCharacter::SearchTargetToSnap()
{
  TargetToSnap = nullptr;

  // FIXME Same as RotateCharacter_Charge. Make it DRY
  TargetSnapInput.Normalize();
  const FRotator curtPlayerDir_Rot = GetActorRotation();
  const FVector curtPlayerDir = GetActorForwardVector();
  const float TEMP_RANGE = 45.f; 

  // Use ForwardVector as Axis-Y to calculate input direction         
  FVector targetPlayerDir = curtPlayerDir * TargetSnapInput.Y + /*Rotate 90°*/ FVector{curtPlayerDir.Y, -curtPlayerDir.X, 0.0} * TargetSnapInput.X;
  targetPlayerDir.Normalize();
  
  double rotateAngle = FVector::DotProduct(curtPlayerDir, targetPlayerDir);
  if (rotateAngle < FMath::Cos(FMath::DegreesToRadians(TEMP_RANGE)))
  {
    rotateAngle = FMath::Sign(TargetSnapInput.X) * TEMP_RANGE;
  }

  // Rotate Target direction with adjusted angle
  const float TEMP_DETECT_LENGTH = 1000.0f;
  targetPlayerDir = curtPlayerDir.RotateAngleAxis(rotateAngle, curtPlayerDir);
  const FVector startLoc = GetActorLocation();
  const FVector endLoc = startLoc + targetPlayerDir * TEMP_DETECT_LENGTH;

  float radius = 200.f;
  if (UCapsuleComponent* capsule = GetCapsuleComponent())
  {
    radius = capsule->GetScaledCapsuleHalfHeight() * 2.f;
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
                                            EDrawDebugTrace::None,
                                            outResults,
                                            true          // bIgnoreSelf
                                          ); 
  
  if (bHit)
  {
    UClass* targetClass = (TargetClass != nullptr) ? TargetClass->GetClass() : AEnemy_Zako::StaticClass();
    const FVector playerLoc = startLoc;
    for (const FHitResult& hitResult : outResults)
    {
      if (hitResult.GetActor()->IsA(targetClass))
      {
        const float curtHitResultDistanceSquared = (playerLoc - hitResult.GetActor()->GetActorLocation()).SquaredLength();
        if (TargetToSnap != nullptr)
        {
          // Find min distance to player
          const float curtMinDistanceSquared = (playerLoc - TargetToSnap->GetActorLocation()).SquaredLength();
          if (curtMinDistanceSquared > curtHitResultDistanceSquared)
          {
            TargetToSnap = hitResult.GetActor();
          }   
        }
        else
        {
          TargetToSnap = hitResult.GetActor();
        }
      }
    }
  }
}