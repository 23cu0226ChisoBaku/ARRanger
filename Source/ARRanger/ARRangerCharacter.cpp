#include "ARRangerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AARRangerCharacter::AARRangerCharacter()
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

		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::ToggleLockOn);
		PlayerInputComponent->BindAxis("SwitchTarget", this, &AARRangerCharacter::SwitchTarget);
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