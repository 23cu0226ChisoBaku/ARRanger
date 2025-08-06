#include "ARRangerCharacter.h"

#include "ARRangerAnimInstance.h"
#include "AttackData.h"
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
#include "PunchCameraShake.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AARRangerCharacter::AARRangerCharacter()
	: DefaultArmLength(250)
	, DashArmLength(500)
	, ArmLengthInterpSpeed(2.5f)
	, maxLockOnDistance(1500.0f)
	, isDashed(false)
	, dashStartThreshold(0.92f)
	, dashEndThreshold(0.7f)
	, isAttacked(false)
	, isAbleToSwitchTarget(false)
	, isAttractingEnemy(false)
	, isStrongAttack(false)
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
}

void AARRangerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance found! Registering OnMontageEnded"));
		// AnimInstanceにバインド
		AnimInstance->OnMontageEnded.AddDynamic(this, &AARRangerCharacter::OnAttackMontageEnded);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at BeginPlay!"));
	}

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
}

// 麦
void AARRangerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  // 麦
  LandedDelegate.RemoveDynamic(this, &AARRangerCharacter::LandedToGround);
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
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::ToggleLockOn);

		// ターゲット切り替え(右、左)
		EnhancedInputComponent->BindAction(SwitchTargetRightAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::SwitchTargetRight);
		EnhancedInputComponent->BindAction(SwitchTargetLeftAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::SwitchTargetLeft);

		// 攻撃(パンチ、キック)
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AARRangerCharacter::StartPunch);
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Started, this, &AARRangerCharacter::Kick);

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
	if (!isDashed && InputMagnitude > dashStartThreshold)
	{
		isDashed = true;
	}
	else if (isDashed && InputMagnitude < dashEndThreshold)
	{
		isDashed = false;
	}

	// ロックオン中に処理
	if (isLockedOn && LockedOnTarget)
	{
		FVector ToTarget = LockedOnTarget->GetActorLocation() - GetActorLocation();
		FRotator TargetRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;

		if (!IsTargetVisible(LockedOnTarget))
		{
			// 敵がプレイヤーから見えなくなったらロックオン解除
			LockedOnTarget = nullptr;
			isLockedOn = false;
			UE_LOG(LogTemp, Warning, TEXT("Lost lock-on because target is not visible."))
		}

		// ロックオン中の敵が消えたら処理
		if (!IsValid(LockedOnTarget) || LockedOnTarget->IsActorBeingDestroyed())
		{
			AActor* NewTarget = FindNearestEnemy(LockedOnTarget);
			// 新しくターゲットを設定
			if (NewTarget)
			{
				LockedOnTarget = NewTarget;
			}
			// いなければロックオン解除
			else
			{
				LockedOnTarget = nullptr;
				isLockedOn = false;
			}
		}
		// ターゲットに向けて回転
		SetActorRotation(TargetRotation);

		// コントローラーも回転する
		if (Controller)
		{
			FRotator CurrentControlRot = Controller->GetControlRotation();

			// ?X???[?Y????
			FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetRotation, DeltaTime, 5.0f);

			Controller->SetControlRotation(NewControlRot);
		}
	}

	// 引き寄せ中に処理
	if (isAttractingEnemy && LockedOnTarget && IsValid(LockedOnTarget))
	{
		FVector PlayerLocation = GetActorLocation();
		FVector EnemyLocation = LockedOnTarget->GetActorLocation();
		FVector Direction = (PlayerLocation - EnemyLocation);
		float Distance = Direction.Size();

		// 最低距離を設定
		const float MinDistance = 150.0f;

		// 最低距離に達したらパンチ
		if (Distance <= MinDistance)
		{
			isAttractingEnemy = false;
			PlayAttackMontage(PunchData); 
			return;
		}

		// 敵を引き寄せる
		float AttractionSpeed = 800.f;
		FVector NewLocation = EnemyLocation + Direction.GetSafeNormal() * AttractionSpeed * DeltaTime;
		LockedOnTarget->SetActorLocation(NewLocation);
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
		FVector End = Start - wallNormal * 100.0f;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

		// デバッグラインで確認
		DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 2.0f);

		// ライントレースで壁を判定
		// 壁がないか、または引力クライム中に斥力状態に変身したらクライムを解除
		if (!bHit || (CurrentARType != EARMagnetismType::Attraction))
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

void AARRangerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() == nullptr || isAttacked || isStrongAttack)
	{
		return;
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
		if (!currentClimbSurface)
		{
			UE_LOG(LogTemp, Warning, TEXT("currentClimbSurface is null!"));
			return;
		}

		if (UARRangerAnimInstance* MyAnim = Cast<UARRangerAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			// 引力クライム中の速度に入力値を反映
			MyAnim->ClimbUpSpeed = Forward;
			MyAnim->ClimbRightSpeed = Right;
		}

		// 壁の向きに対して上下左右に移動
		AddMovementInput(GetActorForwardVector(), Forward);
		AddMovementInput(GetActorRightVector(), Right);
	}
}

void AARRangerCharacter::StartClimbing(AInsekiClimbingObject* ClimbActor)
{
	// クライム中でない、引力クライムオブジェクトに触れていない、または引力状態でないなら処理しない
	if (isClimbed || !ClimbActor || CurrentARType != EARMagnetismType::Attraction)
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
	// 攻撃中は処理しない
	if (isAttacked || isStrongAttack)
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

bool AARRangerCharacter::IsTargetVisible(AActor* Target)
{
	// 敵がいないときは処理しない
	if (!Target)
	{
		return false;
	}
		
	// プレイヤーの視点を取得
	FVector PlayerViewLocation;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewLocation, PlayerViewRotation);

	FVector TargetLocation = Target->GetActorLocation();
	// 少し高さを調整（敵の中心や頭部付近を狙う）
	TargetLocation.Z += 50.0f;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	// 自分自身とターゲットは無視する
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Target);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		PlayerViewLocation,
		TargetLocation,
		ECC_Visibility,
		Params
	);

	// 何かに遮られてヒットした場合は見えていないと判断
	if (bHit)
	{
		// ヒットしたActorがターゲットでなければ遮られていると判定
		if (HitResult.GetActor() != Target)
		{
			return false;
		}
	}

	return true;
}

void AARRangerCharacter::ToggleLockOn()
{
	// 引力クライム中は処理しない
	if (isClimbed)
	{
		return;
	}

	if (isLockedOn)
	{
		// ロックオン解除
		LockedOnTarget = nullptr;
		isLockedOn = false;
	}
	else
	{
		AActor* Candidate = FindNearestEnemy();
		if (Candidate && IsTargetVisible(Candidate))
		{
			// 敵をロックオン
			LockedOnTarget = Candidate;
			isLockedOn = true;
		}
		else
		{
			// 見えていなければロックオン不可
			UE_LOG(LogTemp, Warning, TEXT("Target not visible, cannot lock on."));
		}
	}
}

void AARRangerCharacter::SwitchTargetRight()
{
	// ターゲット切り替え処理
	SwitchTarget(true); 
}

void AARRangerCharacter::SwitchTargetLeft()
{
	// ターゲット切り替え処理
	SwitchTarget(false); 
}

void AARRangerCharacter::SwitchTarget(bool isPressedRight)
{
	// ロックオン中でなければ処理しない
	if (!isLockedOn)
	{
		return;
	}
		
	// ワールドの敵を取得(To Do：範囲内の敵を取得に変更する)
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);
	if (Enemies.Num() <= 1)
	{
		return;
	}
	int32 CurrentIndex = Enemies.IndexOfByKey(LockedOnTarget);
	if (CurrentIndex == INDEX_NONE)
	{
		return;
	}

	// プレイヤーの位置を取得
	const FVector MyLocation = GetActorLocation();

	const int32 EnemyCount = Enemies.Num();
	int32 Index = CurrentIndex;
	int32 Checked = 0;

	while (Checked < EnemyCount)
	{
		// 敵の数をチェック
		Index = isPressedRight
			? (Index + 1) % EnemyCount
			: (Index - 1 + EnemyCount) % EnemyCount;

		// 現在選択中の敵になったら処理をやめる
		if (Index == CurrentIndex)
		{
			break;
		}

		AActor* Candidate = Enemies[Index];
		if (!Candidate)
		{
			Checked++;
			continue;
		}

		const float Distance = FVector::Dist(MyLocation, Candidate->GetActorLocation());
		if (Distance <= maxLockOnDistance)
		{
			LockedOnTarget = Candidate;
			return;
		}

		Checked++;
	}
}

AActor* AARRangerCharacter::FindNearestEnemy(AActor* IgnoreActor)
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);

	AActor* NearestEnemy = nullptr;
	float MinDistSq = FLT_MAX;
	FVector MyLocation = GetActorLocation();
	float MaxDistSq = maxLockOnDistance * maxLockOnDistance;

	for (AActor* Enemy : Enemies)
	{
		if (Enemy == IgnoreActor || !IsValid(Enemy))
		{
			continue;
		}
			
		float DistSq = FVector::DistSquared(MyLocation, Enemy->GetActorLocation());

		if (DistSq <= MaxDistSq && DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			NearestEnemy = Enemy;
		}
	}
	return NearestEnemy;
}

void AARRangerCharacter::StartPunch()
{
	// 引力クライム中は処理しない
	if (isClimbed)
	{
		return;
	}

	// 引力状態で敵をロックオンしていれば処理
	if (CurrentARType == EARMagnetismType::Attraction && isLockedOn && LockedOnTarget)
	{
		if (!isAttractingEnemy)
		{
			// 引き寄せを開始
			isAttractingEnemy = true;
			isStrongAttack = true;

			if (PunchData.Montage_AR && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(PunchData.Montage_AR))
			{
				GetMesh()->GetAnimInstance()->Montage_Play(PunchData.Montage_AR);
			}
		}
		return;
	}

	// 通常パンチ処理
	isStrongAttack = false;
	PlayAttackMontage(PunchData);
}

void AARRangerCharacter::PunchHitNotify()
{
	AttackHit(PunchData);
}

void AARRangerCharacter::Kick()
{
	// 引力クライム中は処理しない
	if (isClimbed)
	{
		return;
	}

	PlayAttackMontage(KickData);
}

void AARRangerCharacter::KickHitNotify()
{
	AttackHit(KickData);
}

void AARRangerCharacter::PlayAttackMontage(const FAttackData& Attack)
{
	// アニメーションがない、または攻撃中は処理しない
	if (!Attack.Montage_Normal || !Attack.Montage_Strong || isAttacked)
	{
		return;
	}

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	// 攻撃アニメーション再生中は処理しない
	if (!Anim || Anim->Montage_IsPlaying(Attack.Montage_Normal) || Anim->Montage_IsPlaying(Attack.Montage_Strong))
	{
		return;
	}

	isAttacked = true;

	// 強攻撃アニメーションを再生するか判断
	if (isStrongAttack)
	{
		Anim->Montage_Play(Attack.Montage_Strong);
	}
	else
	{
		Anim->Montage_Play(Attack.Montage_Normal);
	}
}

void AARRangerCharacter::AttackHit(const FAttackData& Attack)
{
	FVector Origin = GetActorLocation() + GetActorForwardVector() * 100.f;
	TArray<AActor*> HitActors;

	// 当たり判定を作成
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		this,
		Origin,
		Attack.HitRadius,
		TArray<TEnumAsByte<EObjectTypeQuery>>{
		UEngineTypes::ConvertToObjectType(ECC_Pawn),
			UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)
	},
		nullptr,
		TArray<AActor*>{this},
		HitActors
	);

	if (!bHit)
	{
		return;
	}
		
	for (AActor* HitActor : HitActors)
	{
		if (HitActor->ActorHasTag(Attack.TargetTag))
		{
			AEnemy* Enemy = Cast<AEnemy>(HitActor);
			if (Enemy && !Enemy->isDead)
			{
				TSharedRef<ARRanger::INotifyHandlerInterface> notifyHandler = GetNotifyHandlerRef();
				notifyHandler->OnAttack();

				const bool bWillBeKilled = (Enemy->currentHP - Attack.Damage <= 0);

				FVector LaunchDir = GetActorForwardVector() + FVector(0, 0, 0.2f);
				LaunchDir.Normalize();

				// ダメージを与える(強攻撃ならダメージを上乗せ)
				if (isStrongAttack)
				{
					Enemy->ReceiveDamage(Attack.Damage + Attack.DamageModifier, LaunchDir, bWillBeKilled);
				}
				else
				{
					Enemy->ReceiveDamage(Attack.Damage, LaunchDir, bWillBeKilled);
				}
			}
		}
	}
}

void AARRangerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	isAttacked = false;
	isStrongAttack = false;
}

void AARRangerCharacter::Transform()
{
	// 攻撃中は処理しない
	if (isAttacked || isStrongAttack)
	{
		return;
	}

	// 現在と別のモードに変身
	CurrentARType = (CurrentARType == EARMagnetismType::Attraction)
		? EARMagnetismType::Repulsion
		: EARMagnetismType::Attraction;

	// メッシュを別モードに変更
	USkeletalMesh* NewMesh = (CurrentARType == EARMagnetismType::Repulsion)
		? RepulsionMesh
		: AttractionMesh;

	if (NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
}

EARMagnetismType AARRangerCharacter::GetCurrentARType()
{
	return CurrentARType;
}