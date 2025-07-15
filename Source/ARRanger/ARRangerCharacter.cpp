#include "ARRangerCharacter.h"

#include "Camera/CameraComponent.h"
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
#include "Kismet/KismetSystemLibrary.h"
#include "PunchCameraShake.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AARRangerCharacter::AARRangerCharacter()
	: DefaultArmLength(250)
	, DashArmLength(500)
	, ArmLengthInterpSpeed(2.5f)
	, maxLockOnDistance(1500.0f)
	, isDashed(false)
	, moveThreshold(0.9f)
	, attackPower(35)
	, punchRadius(100.0f)
	, kickRadius(120.0f)
	, isAttacked(false)
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

	// �J�����u�[�������i�Փ˂��������ꍇ�A�v���C���[�̕��Ɉ����񂹂���j
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// �t�H���[�J�����̍쐬
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

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

		// ����
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ���b�N�I��
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::ToggleLockOn);

		// ���b�N�I�����^�[�Q�b�g�؂�ւ�
		EnhancedInputComponent->BindAction(SwitchTargetAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::SwitchTarget);

		// �p���`
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AARRangerCharacter::Punch);

		// �L�b�N
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Started, this, &AARRangerCharacter::Kick);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AARRangerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �v���C���[��G�̕����Ɍ�����
	if (bIsLockedOn && LockedOnTarget)
	{
		FVector Direction = LockedOnTarget->GetActorLocation() - GetActorLocation();
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		NewRotation.Pitch = 0.f;
		NewRotation.Roll = 0.f;
		SetActorRotation(NewRotation);
	}

	// �_�b�V�����Ă��邩�̔���
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				const FInputActionValue InputValue = Subsystem->GetPlayerInput()->GetActionValue(MoveAction);
				if (InputValue.GetValueType() == EInputActionValueType::Axis2D)
				{
					FVector2D InputVec = InputValue.Get<FVector2D>();
					// ���͒l��臒l�𒴂��Ă��邩�Ŕ��f
					isDashed = InputVec.Size() > moveThreshold; 
				}
			}
		}
	}

	// �_�b�V�����ɃJ�������v���C���[�ɋ߂Â���
	float TargetArmLength = isDashed ? DashArmLength : DefaultArmLength;
	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		ArmLengthInterpSpeed);

	// ���b�N�I�����̏���
	if (bIsLockedOn && LockedOnTarget)
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
				bIsLockedOn = false;
			}
		}
		// �L�����N�^�[�{�̂���]������
		SetActorRotation(TargetRotation);

		// �J�����i�R���g���[���[�j�����������։�]������
		if (Controller)
		{
			FRotator CurrentControlRot = Controller->GetControlRotation();

			// �X���[�Y�ɕ�ԁi�I�v�V�����j
			FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetRotation, DeltaTime, 5.0f);

			Controller->SetControlRotation(NewControlRot);
		}

		
	}
}

void AARRangerCharacter::Move(const FInputActionValue& Value)
{
	// ���͂�Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

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

void AARRangerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// �U�����͈ړ����Ȃ�
		if (isAttacked)
		{
			return;
		}

		// ���̓x�N�g���̒����ő����Ă��邩�ǂ�������
		const float InputMagnitude = FVector2D(Right, Forward).Size();
		isDashed = InputMagnitude > moveThreshold;

		// �ǂ���������Ă��邩���ׂ�
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// �O���x�N�g���̎擾
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// �E�����x�N�g���̎擾
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add Movement
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
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
	if (isAttacked)
	{
		return;
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
	if (bIsLockedOn)
	{
		// ���b�N�I������
		UE_LOG(LogTemp, Warning, TEXT("Lock off"));
		LockedOnTarget = nullptr;
		bIsLockedOn = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LockOoooooooooon"));
		// �ł��߂��G���擾
		LockedOnTarget = FindNearestEnemy();
		if (LockedOnTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Locked on to: %s"), *LockedOnTarget->GetName());
			bIsLockedOn = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No enemies to lock onto"));
		}
	}
}

void AARRangerCharacter::SwitchTarget(const FInputActionValue& Value)
{
	if (!bIsLockedOn) return;

	float AxisValue = Value.Get<float>();

	// 臒l����i�X�e�B�b�N�y���|�ꂽ�����ł͐؂�ւ��Ȃ��悤�Ɂj
	if (FMath::Abs(AxisValue) < 0.5f)
		return;

	// �����ɉ����Đ؂�ւ�
	const bool bRight = AxisValue > 0;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);
	if (Enemies.Num() <= 1) return;

	int32 CurrentIndex = Enemies.IndexOfByKey(LockedOnTarget);
	if (CurrentIndex == INDEX_NONE) return;

	int32 NewIndex = bRight
		? (CurrentIndex + 1) % Enemies.Num()
		: (CurrentIndex - 1 + Enemies.Num()) % Enemies.Num();

	LockedOnTarget = Enemies[NewIndex];
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
		if (Enemy == IgnoreActor || !IsValid(Enemy)) continue;

		float DistSq = FVector::DistSquared(MyLocation, Enemy->GetActorLocation());

		if (DistSq <= MaxDistSq && DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			NearestEnemy = Enemy;
		}
	}
	return NearestEnemy;
}


void AARRangerCharacter::Punch()
{
	if (PunchMontage && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(PunchMontage))
	{
		// �U�����t���O���グ��
		isAttacked = true;

		// �p���`�A�j���[�V�������Đ�
		GetMesh()->GetAnimInstance()->Montage_Play(PunchMontage);
	}
}

void AARRangerCharacter::PunchHitNotify()
{
	FVector Origin = GetActorLocation() + GetActorForwardVector() * 100.f;
	//DrawDebugSphere(GetWorld(), Origin, punchRadius, 16, FColor::Red, false, 2.0f);
	TArray<AActor*> OverlappingActors;

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		this,
		Origin,
		punchRadius,
		TArray<TEnumAsByte<EObjectTypeQuery>>{UEngineTypes::ConvertToObjectType(ECC_Pawn)},
		nullptr,
		TArray<AActor*>{this}, // ���������O
		OverlappingActors
	);

	if (!bHit) return;

	for (AActor* HitActor : OverlappingActors)
	{
		if (HitActor->ActorHasTag("Enemy"))
		{
			AEnemy* Enemy = Cast<AEnemy>(HitActor);
			if (Enemy)
			{
				// ���S���Ă����珈�����Ȃ�
				if (Enemy->isDead) continue;

				FVector LaunchDir = GetActorForwardVector() + FVector(0, 0, 0.2f);
				LaunchDir.Normalize();

				// �G��HP��0�ɂȂ邩�ǂ�����Ɍ���
				bool willBeKilled = (Enemy->currentHP - attackPower <= 0);

				Enemy->ReceiveDamage(attackPower, LaunchDir, willBeKilled);
			}
		}
	}
}

void AARRangerCharacter::Kick()
{
	if (KickMontage && !GetMesh()->GetAnimInstance()->Montage_IsPlaying(KickMontage))
	{
		// �U�����t���O���グ��
		isAttacked = true;

		// �L�b�N�A�j���[�V�������Đ�
		GetMesh()->GetAnimInstance()->Montage_Play(KickMontage);
	}
}

void AARRangerCharacter::KickHitNotify()
{
	// �����蔻����쐬
	// �p���`��菭���L������
	FVector Origin = GetActorLocation() + GetActorForwardVector() * 100.f;

	TArray<AActor*> OverlappingActors;
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		this,
		Origin,
		kickRadius,
		TArray<TEnumAsByte<EObjectTypeQuery>>{
		UEngineTypes::ConvertToObjectType(ECC_Pawn),
			UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)
	},
		nullptr,
		TArray<AActor*>{this},
		OverlappingActors
	);

	if (!bHit) return;

	for (AActor* HitActor : OverlappingActors)
	{
		if (HitActor->ActorHasTag("Enemy"))
		{
			AEnemy* Enemy = Cast<AEnemy>(HitActor);
			if (Enemy)
			{
				// ���S���Ă����珈�����Ȃ�
				if (Enemy->isDead) continue;

				FVector LaunchDir = GetActorForwardVector() + FVector(0, 0, 0.2f);
				LaunchDir.Normalize();

				// �G��HP��0�ɂȂ邩�ǂ�����Ɍ���
				bool willBeKilled = (Enemy->currentHP - attackPower <= 0);

				Enemy->ReceiveDamage(attackPower, LaunchDir, willBeKilled);
			}
		}
	}
}

void AARRangerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	isAttacked = false;
}