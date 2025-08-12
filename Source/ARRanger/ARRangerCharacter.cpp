#include "ARRangerCharacter.h"

#include "ARRangerAnimInstance.h"
#include "AttackComponent.h"
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
#include "PunchCameraShake.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AARRangerCharacter::AARRangerCharacter()
	: DefaultArmLength(250)
	, DashArmLength(500)
	, ArmLengthInterpSpeed(2.5f)
	, IsDashed(false)
	, dashStartThreshold(0.92f)
	, dashEndThreshold(0.7f)
	, LockOnComponent(nullptr)
	, currentClimbSurface(nullptr)
	, wallNormal(0.0f, 0.0f, 0.0f)
	, isClimbed(false)
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
	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));
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
	// 各アクションのバインド
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// ジャンプ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AARRangerCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AARRangerCharacter::DoJumpEnd);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Move);

		// カメラ回転
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ロックオン
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, LockOnComponent, &ULockOnComponent::ToggleLockOn);

		// ターゲット切り替え(右、左)
		EnhancedInputComponent->BindAction(SwitchTargetRightAction, ETriggerEvent::Triggered, LockOnComponent, &ULockOnComponent::SwitchTargetRight);
		EnhancedInputComponent->BindAction(SwitchTargetLeftAction, ETriggerEvent::Triggered, LockOnComponent, &ULockOnComponent::SwitchTargetLeft);

		// 攻撃(パンチ、キック)
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, AttackComponent, &UAttackComponent::StartPunch);
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Started, AttackComponent, &UAttackComponent::StartKick);

		// 変身
		EnhancedInputComponent->BindAction(TransformAction, ETriggerEvent::Started, this, &AARRangerCharacter::Transform);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AARRangerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 入力値を取得
	float InputMagnitude = 0.f;
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = Cast<ULocalPlayer>(PC->Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				const FInputActionValue InputValue = Subsystem->GetPlayerInput()->GetActionValue(MoveAction);
				if (InputValue.GetValueType() == EInputActionValueType::Axis2D)
				{
					InputMagnitude = InputValue.Get<FVector2D>().Size();
				}
			}
		}
	}

	// ヒステリシスを用いてダッシュ判定
	if (!IsDashed && InputMagnitude > dashStartThreshold)
	{
		IsDashed = true;
	}
	else if (IsDashed && InputMagnitude < dashEndThreshold)
	{
		IsDashed = false;
	}

	// 移動入力の更新
	UpdateMovementState();

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
	if (isClimbed)
	{
		// DoMoveが呼ばれない際はここで入力値を反映
		const FVector Input = GetLastMovementInputVector();
		if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			MyAnim->ClimbUpSpeed = Input.Y;
			MyAnim->ClimbRightSpeed = Input.X;
		}

		// 壁回転処理
		// 足元の位置（Capsuleの底の位置）
		UCapsuleComponent* Capsule = GetCapsuleComponent();
		FVector ActorLocation = GetActorLocation();
		float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		// 壁に対して垂直な向きに少しめり込むようにして設定
		FVector FootPosition = ActorLocation - wallNormal * (HalfHeight - 5.0f);

		FVector Start = FootPosition;
		FVector End = Start - wallNormal * 70.0f;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		// デバッグラインで確認
		DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 2.0f);

		// ライントレースで壁を判定
		// 壁がないか、または引力クライム中に斥力状態に変身したらクライムを解除
		if (!bHit || (GetMagnetismType() != EARMagnetismType::Attraction))
		{
			// クライム解除＋ジャンプ処理
			isClimbed = false;
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

void AARRangerCharacter::Move(const FInputActionValue& Value)
{
	// 入力値を取得
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	// 移動処理
	DoMove(MovementVector.X, MovementVector.Y);
}

void AARRangerCharacter::Look(const FInputActionValue& Value)
{
	// 入力値を取得
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// カメラ回転処理
	DoLook(LookAxisVector.X, LookAxisVector.Y);
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

void AARRangerCharacter::UpdateMovementState()
{
	bool bIsMovingNow = false;
	if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		bIsMovingNow = MyAnim->ShouldMove;
	}

	// 歩き終了を検知
	if (bWasMoving && !bIsMovingNow)
	{
		// 歩き終了した瞬間に呼ぶ
		if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			MyAnim->RequestFinishWalk();
		}
	}

	bWasMoving = bIsMovingNow;
}


void AARRangerCharacter::DoMove(float Right, float Forward)
{
	bool isAttacked =AttackComponent->GetIsAttacked();
	bool isStrongAttacked = AttackComponent->GetIsStrongAttacked();
	bool isAttractingEnemy = AttackComponent->GetIsAttractingEnemy();

	// コントローラーがない、引き寄せ中または攻撃中なら処理しない
	if (GetController() == nullptr || isAttractingEnemy || isAttacked || isStrongAttacked)
	{
		return;
	}

	// 入力閾値
	const float InputDeadZone = 0.2f;

	// 微小な入力はゼロにする
	if (FMath::Abs(Right) < InputDeadZone) Right = 0.f;
	if (FMath::Abs(Forward) < InputDeadZone) Forward = 0.f;

	bool hasInput = (FMath::Abs(Right) > KINDA_SMALL_NUMBER || FMath::Abs(Forward) > KINDA_SMALL_NUMBER);
	UARRangerAnimInstance* AnimInst = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		if (hasInput)
		{
			AnimInst->ShouldMove = true;  // 入力あれば歩く
		}
		else if (AnimInst->ShouldMove && !AnimInst->FinishWalk)
		{
			AnimInst->RequestFinishWalk();  // 入力なくなった瞬間、半歩歩き残す処理へ
		}
	}

	if (!isClimbed)
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

		// 壁に対して上下左右に動かす
		AddMovementInput(GetActorForwardVector(), Forward);
		AddMovementInput(GetActorRightVector(), Right);
	}
}

void AARRangerCharacter::StartClimbing(AInsekiClimbingObject* ClimbActor)
{
	// クライム中でない、引力クライムオブジェクトに触れていない、または引力状態でないなら処理しない
	if (isClimbed || !ClimbActor || GetMagnetismType() != EARMagnetismType::Attraction)
	{
		return;
	}
		
	// 引力クライムフラグを上げる
	isClimbed = true;
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
		// 壁に対して垂直になるようキャラを回転させる
		const FVector X = GetActorUpVector().GetSafeNormal();
		const FVector Z = HitResult.Normal.GetSafeNormal();
		const FRotator NewRot = FRotationMatrix::MakeFromXZ(X, Z).Rotator();
		SetActorRotation(NewRot);

		// 壁の法線を保存
		wallNormal = HitResult.ImpactNormal;
	}
}

void AARRangerCharacter::StopClimbing()
{
	// 引力クライム中でないなら処理しない
	if (!isClimbed)
	{
		return;
	}
		
	// 引力クライムフラグを下げる
	isClimbed = false;
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
	bool isAttacked = AttackComponent->GetIsAttacked();
	bool isStrongAttacked = AttackComponent->GetIsStrongAttacked();

	// 攻撃中は処理しない
	if (isAttacked || isStrongAttacked)
	{
		return;
	}

	// 引力クライムを解除
	if (isClimbed)
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

void AARRangerCharacter::OnAttackHitNotify()
{
	// プレイヤー内でのみ扱いたいのでこちらで攻撃のコールバック
	TSharedRef<ARRanger::INotifyHandlerInterface> notifyHandler = GetNotifyHandlerRef();
	notifyHandler->OnAttack();
}

void AARRangerCharacter::Transform()
{
	bool isAttacked = AttackComponent->GetIsAttacked();
	bool isStrongAttacked = AttackComponent->GetIsStrongAttacked();

	// 攻撃中は処理しない
	if (isAttacked || isStrongAttacked)
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
}

EARMagnetismType AARRangerCharacter::GetCurrentARType()
{
	return GetMagnetismType();
}

void AARRangerCharacter::OnMagneticForceFieldBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
	{
		Physics_RequestMagneticTask(this, magnetizableObj);
	}
}

void AARRangerCharacter::OnMagneticForceFieldEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
	{
		Physics_TerminateMagneticTask(this, magnetizableObj);
	}
}

void AARRangerCharacter::OnMagnetizedObjectHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IARMagnetizableInterface* magnetizableObj = Cast<IARMagnetizableInterface>(OtherActor))
	{
		Physics_RequestMagneticTask_Once(this, magnetizableObj);
	}
}