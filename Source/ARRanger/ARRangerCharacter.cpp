#include "ARRangerCharacter.h"

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
	, isClimb(false)
	, isClimbed(false)
{
	// �J�v�Z���̃T�C�Y��ݒ肷��
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// �R���g���[���[����]���Ă���]�����Ȃ��B�J�����ɉe����^���邾���ɂ���
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// �L�����N�^�[�̓�����ݒ肷��
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// ��: �������Ԃ�Z�k���邽�߂ɁA�����̕ϐ��₻�̑������̕ϐ����A�ăR���p�C�����Ē�������̂ł͂Ȃ��A�L�����N�^�[�u���[�v�����g
    // �Œ������邱�Ƃ��ł���
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;


	// ��: Mesh�R���|�[�l���g (Character����p��) �̃X�P���^�����b�V���ƃA�j���[�V�����u���[�v�����g�̎Q�Ƃ́A
    // ThirdPersonCharacter�Ƃ������O�̔h���u���[�v�����g�A�Z�b�g�ɐݒ肳��� (C++ �ł̃R���e���c�̒��ڎQ�Ƃ�����邽��)�B
}

void AARRangerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance found! Registering OnMontageEnded"));
		// �U���A�j���[�V�����̏I�����AOnAttackMontageEnded���Ă΂��悤�ɂ���
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
	// �A�N�V�����o�C���f�B���O�̐ݒ�
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// �W�����v
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AARRangerCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AARRangerCharacter::DoJumpEnd);

		// �ړ�
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ���_�ړ�
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ���b�N�I��
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::ToggleLockOn);

		// ���b�N�I�����^�[�Q�b�g�؂�ւ�(���̃^�[�Q�b�g)
		EnhancedInputComponent->BindAction(SwitchTargetRightAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::SwitchTargetRight);

		// ���b�N�I�����^�[�Q�b�g�؂�ւ�(�O�̃^�[�Q�b�g)
		EnhancedInputComponent->BindAction(SwitchTargetLeftAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::SwitchTargetLeft);

		// �p���`
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AARRangerCharacter::StartPunch);

		// �L�b�N
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Started, this, &AARRangerCharacter::Kick);

		// �ϐg
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


	// ���t���[�����͋��x���`�F�b�N����isDashed���X�V
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

	// �q�X�e���V�X�ɂ��_�b�V������
	if (!isDashed && InputMagnitude > dashStartThreshold)
	{
		isDashed = true;
	}
	else if (isDashed && InputMagnitude < dashEndThreshold)
	{
		isDashed = false;
	}

	// ���b�N�I�����̏���
	if (isLockedOn && LockedOnTarget)
	{
		FVector ToTarget = LockedOnTarget->GetActorLocation() - GetActorLocation();
		FRotator TargetRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;

		if (!IsValid(LockedOnTarget) || LockedOnTarget->IsActorBeingDestroyed())
		{
			AActor* NewTarget = FindNearestEnemy(LockedOnTarget);
			// ���b�N�I�������G��|������A�߂��ɓG������΂�����Ƀ��b�N�I������
			if (NewTarget)
			{
				LockedOnTarget = NewTarget;
			}
			// ���Ȃ���΃��b�N�I��������
			else
			{
				LockedOnTarget = nullptr;
				isLockedOn = false;
			}
		}
		// �L�����N�^�[�{�̂���]������
		SetActorRotation(TargetRotation);

		// �J�����i�R���g���[���[�j�����������։�]������
		if (Controller)
		{
			FRotator CurrentControlRot = Controller->GetControlRotation();

			// �X���[�Y�ɕ��
			FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetRotation, DeltaTime, 5.0f);

			Controller->SetControlRotation(NewControlRot);
		}
	}

	if (isAttractingEnemy && LockedOnTarget && IsValid(LockedOnTarget))
	{
		FVector PlayerLocation = GetActorLocation();
		FVector EnemyLocation = LockedOnTarget->GetActorLocation();
		FVector Direction = (PlayerLocation - EnemyLocation);
		float Distance = Direction.Size();

		// �����񂹏I������
		const float MinDistance = 150.0f;

		// �����񂹂�����������p���`���s��
		if (Distance <= MinDistance)
		{
			isAttractingEnemy = false;
			PlayAttackMontage(PunchData); 
			return;
		}

		// ���X�ɋ߂Â���i�z���X�s�[�h�����j
		float AttractionSpeed = 800.f;
		FVector NewLocation = EnemyLocation + Direction.GetSafeNormal() * AttractionSpeed * DeltaTime;
		LockedOnTarget->SetActorLocation(NewLocation);
	}

	// 引力クライム中に処理
	if (isClimbed)
	{
		// まずMovementModeをFlyingに設定
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

		// LineTraceで壁を調べる
		FHitResult HitResult;
		FVector Start = GetActorLocation();
		FVector End = Start + GetActorForwardVector() * 100.0f; // 前方100cmにトレース

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
		{
			const FVector X = GetActorUpVector().GetSafeNormal();
			const FVector Z = HitResult.Normal.GetSafeNormal();
			const FRotator NewRot = FRotationMatrix::MakeFromXZ(X, Z).Rotator();
			SetActorRotation(NewRot);
		}
	}
}

void AARRangerCharacter::Move(const FInputActionValue& Value)
{
	// ���͂�Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// �R��
	if (isClimb)
	{
		// �R��
		DoClimb(MovementVector.X, MovementVector.Y);
		return;
	}
	
		// ���͂����[�e�B���O����
		DoMove(MovementVector.X, MovementVector.Y);
}

void AARRangerCharacter::Look(const FInputActionValue& Value)
{
	// ���͂�Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// ���͂����[�e�B���O����
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

// �R��
void AARRangerCharacter::DoClimb(float Right, float Up)
{
	if (GetController() != nullptr)
	{
		// �ǂ���������Ă��邩���ׂ�
		FRotator YawRotation(0, GetActorRotation().Yaw, 0);

		FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(FVector::UpVector, Up);
		AddMovementInput(RightDir, Right);
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

void AARRangerCharacter::StartClimbing(AInsekiClimbingObject* ClimbActor)
{
	// クライム中でない、引力クライムオブジェクトに触れていない、または引力状態でないなら処理しない
	if (isClimbed || !ClimbActor || CurrentGravityType != EGravityType::Attractive)
	{
		return;
	}
		
	// 引力クライムフラグを上げる
	isClimbed = true;
	currentClimbSurface = ClimbActor;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// SurfaceNormalをキャラクターのUpベクトルにする
	/*const FVector SurfaceNormal = currentClimbSurface->GetActorUpVector();

	// キャラクターの前方向を、プレイヤーの視点方向or入力方向から得る
	const FVector Forward = FVector::CrossProduct(GetActorRightVector(), SurfaceNormal);
	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(Forward, SurfaceNormal);
	SetActorRotation(NewRot);

	// 位置補正のため壁に押し付ける
	float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector AttachLocation = GetActorLocation() - SurfaceNormal * (HalfHeight - 1.f);
	SetActorLocation(AttachLocation);*/
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

	// 回転を元に戻す
	SetActorRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
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
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

		// 壁回転処理
		FHitResult HitResult;
		FVector Start = GetActorLocation();
		FVector End = Start + GetActorForwardVector() * 100.0f;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
		{
			const FVector X = GetActorUpVector().GetSafeNormal();
			const FVector Z = HitResult.Normal.GetSafeNormal();
			const FRotator NewRot = FRotationMatrix::MakeFromXZ(X, Z).Rotator();
			SetActorRotation(NewRot);
		}

		if (!currentClimbSurface)
		{
			UE_LOG(LogTemp, Warning, TEXT("currentClimbSurface is null!"));
			return;
		}

		// 壁の法線（壁の外向き方向）
		const FVector WallNormal = -currentClimbSurface->GetActorUpVector().GetSafeNormal();

		// 壁に沿った上方向（ワールドの上と壁法線の外積）
		const FVector WallUp = FVector::CrossProduct(WallNormal, GetActorRightVector()).GetSafeNormal();

		// 壁に沿った右方向（＝壁の上 × 壁の法線）
		const FVector WallRight = FVector::CrossProduct(WallUp, WallNormal).GetSafeNormal();

		// 入力に応じた移動ベクトル（壁に沿った上下・左右）
		const FVector ClimbDirection = (WallUp * Forward + WallRight * Right).GetSafeNormal();

		AddMovementInput(GetActorForwardVector(), Forward);
		AddMovementInput(GetActorRightVector(), Right);
	}
}

void AARRangerCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// �R���g���[���[�Ƀ��[�ƃs�b�`�̓��͂�ǉ�����
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AARRangerCharacter::DoJumpStart()
{
	// �U�����̓W�����v���Ȃ�
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


	// �L�����N�^�[���W�����v���鍇�}
	Jump();
}

void AARRangerCharacter::DoJumpEnd()
{
	// �L�����N�^�[���W�����v����߂鍇�}
	StopJumping();
}

void AARRangerCharacter::ToggleLockOn()
{
	if (isLockedOn)
	{
		// ���b�N�I������
		UE_LOG(LogTemp, Warning, TEXT("Lock off"));
		LockedOnTarget = nullptr;
		isLockedOn = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LockOoooooooooon"));
		// �ł��߂��G���擾
		LockedOnTarget = FindNearestEnemy();
		if (LockedOnTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Locked on to: %s"), *LockedOnTarget->GetName());
			isLockedOn = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No enemies to lock onto"));
		}
	}
}

void AARRangerCharacter::SwitchTargetRight()
{
	// ���̃^�[�Q�b�g��
	SwitchTarget(true); 
}

void AARRangerCharacter::SwitchTargetLeft()
{
	// �O�̃^�[�Q�b�g��
	SwitchTarget(false); 
}

void AARRangerCharacter::SwitchTarget(bool isPressedRight)
{
	// �񃍃b�N�I�����͏������Ȃ�
	if (!isLockedOn)
	{
		return;
	}
		
	// �G�����[���h�ɕ����̂��Ȃ��Ƃ��͏������Ȃ�
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

	// ���g�̈ʒu���擾
	const FVector MyLocation = GetActorLocation();

	const int32 EnemyCount = Enemies.Num();
	int32 Index = CurrentIndex;
	int32 Checked = 0;

	while (Checked < EnemyCount)
	{
		// ���̃C���f�b�N�X������
		Index = isPressedRight
			? (Index + 1) % EnemyCount
			: (Index - 1 + EnemyCount) % EnemyCount;

		// �������g�ɖ߂�����I��
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
	// ���͏�ԏ����b�N�I����Ԃ̎��ɏ���
	if (CurrentGravityType == EGravityType::Attractive && isLockedOn && LockedOnTarget)
	{
		if (!isAttractingEnemy)
		{
			// �����񂹃t���O�Ƌ��U���t���O�𗧂Ă�
			isAttractingEnemy = true;
			isStrongAttack = true;

			if (PunchData.Montage_AR && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(PunchData.Montage_AR))
			{
				GetMesh()->GetAnimInstance()->Montage_Play(PunchData.Montage_AR);
			}
		}
		return;
	}

	// �ʏ�p���`
	isStrongAttack = false;
	PlayAttackMontage(PunchData);
}

void AARRangerCharacter::PunchHitNotify()
{
	AttackHit(PunchData);
}

void AARRangerCharacter::Kick()
{
	PlayAttackMontage(KickData);
}

void AARRangerCharacter::KickHitNotify()
{
	AttackHit(KickData);
}

void AARRangerCharacter::PlayAttackMontage(const FAttackData& Attack)
{
	// Null�`�F�b�N�E�U�����`�F�b�N
	if (!Attack.Montage_Normal || !Attack.Montage_Strong || isAttacked)
	{
		return;
	}

	UAnimInstance* Anim = GetMesh()->GetAnimInstance();
	// �Đ����͏������Ȃ�
	if (!Anim || Anim->Montage_IsPlaying(Attack.Montage_Normal) || Anim->Montage_IsPlaying(Attack.Montage_Strong))
	{
		return;
	}

	isAttacked = true;

	// ���U���t���O�������Ă���΁A���U���A�j���[�V�������Đ�
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

	// �����蔻����쐬
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

				// ���U���t���O�������Ă���΃_���[�W����悹
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
	// �U�����͏������Ȃ�
	if (isAttacked)
	{
		return;
	}

	// ���[�h�ύX�i���� or �˗́j
	CurrentGravityType = (CurrentGravityType == EGravityType::Attractive)
		? EGravityType::Repulsive
		: EGravityType::Attractive;

	// ���f���؂�ւ�
	USkeletalMesh* NewMesh = (CurrentGravityType == EGravityType::Repulsive)
		? RepulsiveMesh
		: AttractiveMesh;

	if (NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
}

EGravityType AARRangerCharacter::GetCurrentGravityType()
{
	return CurrentGravityType;
}