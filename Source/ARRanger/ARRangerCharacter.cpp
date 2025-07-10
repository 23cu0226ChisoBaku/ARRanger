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
	, bIsDashing(false)
	, moveThreshold(0.9f)
	, punchRadius(100.0f)
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

void AARRangerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// �A�N�V�����o�C���f�B���O�̐ݒ�
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// �W�����v
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// �ړ�
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ����
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ���b�N�I��
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::ToggleLockOn);
		PlayerInputComponent->BindAxis("SwitchTarget", this, &AARRangerCharacter::SwitchTarget);

		// �p���`
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AARRangerCharacter::Punch);
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
					bIsDashing = InputVec.Size() > moveThreshold; 
				}
			}
		}
	}

	// �_�b�V�����ɃJ�������v���C���[�ɋ߂Â���
	float TargetArmLength = bIsDashing ? DashArmLength : DefaultArmLength;
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
		// ���̓x�N�g���̒����ő����Ă��邩�ǂ�������
		const float InputMagnitude = FVector2D(Right, Forward).Size();
		bIsDashing = InputMagnitude > moveThreshold;

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

void AARRangerCharacter::SwitchTarget(float Value)
{
	if (!bIsLockedOn || FMath::IsNearlyZero(Value))
		return;

	// ���b�N�I���\�ȓG��S�擾���āA
	// ���݂̃^�[�Q�b�g�̍��E�ɂ���G�ɐ؂�ւ��Ȃ�
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);

	if (Enemies.Num() <= 1) return;

	// ���݂̃^�[�Q�b�g�̃C���f�b�N�X���擾
	int32 CurrentIndex = Enemies.IndexOfByKey(LockedOnTarget);
	if (CurrentIndex == INDEX_NONE) return;

	int32 NewIndex;
	if (Value > 0)
		NewIndex = (CurrentIndex + 1) % Enemies.Num();
	else
		NewIndex = (CurrentIndex - 1 + Enemies.Num()) % Enemies.Num();

	LockedOnTarget = Enemies[NewIndex];
}

AActor* AARRangerCharacter::FindNearestEnemy()
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);

	UE_LOG(LogTemp, Warning, TEXT("Found %d enemies"), Enemies.Num());

	AActor* NearestEnemy = nullptr;
	float MinDistSq = FLT_MAX;
	FVector MyLocation = GetActorLocation();

	for (AActor* Enemy : Enemies)
	{
		float DistSq = FVector::DistSquared(MyLocation, Enemy->GetActorLocation());
		if (DistSq < MinDistSq)
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
		GetMesh()->GetAnimInstance()->Montage_Play(PunchMontage);
	}
}

void AARRangerCharacter::PunchHitNotify()
{
	FVector Origin = GetActorLocation() + GetActorForwardVector() * 100.f;
	DrawDebugSphere(GetWorld(), Origin, punchRadius, 16, FColor::Red, false, 2.0f);
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
			// �X�^�e�B�b�N���b�V�����擾
			if (UStaticMeshComponent* MeshComp = HitActor->FindComponentByClass<UStaticMeshComponent>())
			{
				// �S�̃q�b�g�X�g�b�v�i�X���[�Đ��j
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

				// ���t���[����Ɍ��ɖ߂�
				FTimerHandle HitStopTimerHandle;
				GetWorldTimerManager().SetTimer(HitStopTimerHandle, []()
					{
						UGameplayStatics::SetGlobalTimeDilation(GWorld, 1.0f);
					}, 0.005f, false);

				
				// �J�����V�F�C�N
				if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
				{
					UE_LOG(LogTemp, Warning, TEXT("CameraShake triggered!"));
					if (PlayerController->PlayerCameraManager)
					{
						PlayerController->PlayerCameraManager->StartCameraShake(UPunchCameraShake::StaticClass());
					}
				}

				FVector LaunchDir = GetActorForwardVector() * 600.f + FVector(0, 0, 150.f);
				MeshComp->AddImpulse(LaunchDir, NAME_None, true);
			}
		}
	}
}