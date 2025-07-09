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
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AARRangerCharacter::AARRangerCharacter()
	: CurrentCombo(0)
	, bCanNextCombo(false)
	, bIsAttacking(false)
	, DefaultArmLength(400)
	, DashArmLength(270)
	, ArmLengthInterpSpeed(5.0f)
	, bIsDashing(false)
	, moveThreshold(0.9f)
{
	// カプセルのサイズを設定する
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// コントローラーが回転しても回転させない。カメラに影響を与えるだけにする
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// キャラクターの動きを設定する
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// 注: 反復時間を短縮するために、これらの変数やその他多くの変数を、再コンパイルして調整するのではなく、キャラクターブループリント
    // で調整することができる
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// カメラブームを作る（衝突があった場合、プレイヤーの方に引き寄せられる）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// フォローカメラの作成
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 注: Meshコンポーネント (Characterから継承) のスケルタルメッシュとアニメーションブループリントの参照は、
    // ThirdPersonCharacterという名前の派生ブループリントアセットに設定される (C++ でのコンテンツの直接参照を避けるため)。
}

void AARRangerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// アクションバインディングの設定
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// ジャンプ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// 見る
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ロックオン
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::ToggleLockOn);
		PlayerInputComponent->BindAxis("SwitchTarget", this, &AARRangerCharacter::SwitchTarget);

		// パンチ
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AARRangerCharacter::OnAttackPressed);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AARRangerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// プレイヤーを敵の方向に向ける
	if (bIsLockedOn && LockedOnTarget)
	{
		FVector Direction = LockedOnTarget->GetActorLocation() - GetActorLocation();
		FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		NewRotation.Pitch = 0.f;
		NewRotation.Roll = 0.f;
		SetActorRotation(NewRotation);
	}

	// ダッシュしているかの判定
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
					// 入力値が閾値を超えているかで判断
					bIsDashing = InputVec.Size() > moveThreshold; 
				}
			}
		}
	}

	// ダッシュ時にカメラをプレイヤーに近づける
	float TargetArmLength = bIsDashing ? DashArmLength : DefaultArmLength;
	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		ArmLengthInterpSpeed);

	// ロックオン時の処理
	if (bIsLockedOn && LockedOnTarget)
	{
		FVector ToTarget = LockedOnTarget->GetActorLocation() - GetActorLocation();
		FRotator TargetRotation = FRotationMatrix::MakeFromX(ToTarget).Rotator();
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;

		// キャラクター本体を回転させる
		SetActorRotation(TargetRotation);

		// カメラ（コントローラー）も同じ方向へ回転させる
		if (Controller)
		{
			FRotator CurrentControlRot = Controller->GetControlRotation();

			// スムーズに補間（オプション）
			FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetRotation, DeltaTime, 5.0f);

			Controller->SetControlRotation(NewControlRot);
		}
	}
}

void AARRangerCharacter::Move(const FInputActionValue& Value)
{
	// 入力はVector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// 入力をルーティングする
	DoMove(MovementVector.X, MovementVector.Y);
}

void AARRangerCharacter::Look(const FInputActionValue& Value)
{
	// 入力はVector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// 入力をルーティングする
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AARRangerCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// 入力ベクトルの長さで走っているかどうか判定
		const float InputMagnitude = FVector2D(Right, Forward).Size();
		bIsDashing = InputMagnitude > moveThreshold;

		// どちらを向いているか調べる
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 前方ベクトルの取得
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// 右方向ベクトルの取得
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
		// コントローラーにヨーとピッチの入力を追加する
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AARRangerCharacter::DoJumpStart()
{
	// キャラクターがジャンプする合図
	Jump();
}

void AARRangerCharacter::DoJumpEnd()
{
	// キャラクターがジャンプをやめる合図
	StopJumping();
}

void AARRangerCharacter::ToggleLockOn()
{
	if (bIsLockedOn)
	{
		// ロックオン解除
		UE_LOG(LogTemp, Warning, TEXT("Lock off"));
		LockedOnTarget = nullptr;
		bIsLockedOn = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LockOoooooooooon"));
		// 最も近い敵を取得
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

	// ロックオン可能な敵を全取得して、
	// 現在のターゲットの左右にいる敵に切り替えなど
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);

	if (Enemies.Num() <= 1) return;

	// 現在のターゲットのインデックスを取得
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

void AARRangerCharacter::OnAttackPressed()
{
	if (bIsAttacking)
	{
		if (bCanNextCombo)
		{
			CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, 3);
			bCanNextCombo = false; // 次のコンボを一回だけ許可
		}
		return;
	}

	// 最初の一発目
	CurrentCombo = 1;
	bIsAttacking = true;

	// AnimMontageの該当セクション再生
	PlayComboMontage(CurrentCombo);
}

void AARRangerCharacter::PlayComboMontage(int32 ComboIndex)
{
	if (!PunchMontage || !GetMesh() || !GetMesh()->GetAnimInstance()) return;

	FName SectionName = NAME_None;
	switch (ComboIndex)
	{
	case 1: SectionName = FName("Punch1"); break;
	case 2: SectionName = FName("Punch2"); break;
	case 3: SectionName = FName("Punch3"); break;
	}

	GetMesh()->GetAnimInstance()->Montage_Play(PunchMontage);
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, PunchMontage);
}

void AARRangerCharacter::EnableCombo()
{
	bCanNextCombo = true;
}

void AARRangerCharacter::AttackHitCheck()
{
	FVector HandLocation = GetMesh()->GetSocketLocation("RightHandSocket");
	float Radius = 100.f;

	TArray<AActor*> OverlappedActors;

	// このクラスを除外する
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// 「Pawn」チャンネルでOverlap
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		HandLocation,
		Radius,
		TArray<TEnumAsByte<EObjectTypeQuery>>{UEngineTypes::ConvertToObjectType(ECC_Pawn)},
		nullptr, // 任意でフィルター（AEnemyBase::StaticClass()など）
		ActorsToIgnore,
		OverlappedActors
	);

	if (bHit)
	{
		for (AActor* HitActor : OverlappedActors)
		{
			if (HitActor && HitActor->ActorHasTag("Enemy"))
			{
				// ノックバック処理（3段目）
				if (CurrentCombo == 3)
				{
					ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
					if (HitCharacter)
					{
						FVector LaunchDir = (HitCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
						FVector LaunchVelocity = LaunchDir * 800.0f + FVector(0, 0, 300.0f); // 前＋上方向

						// 吹き飛ばし（Z方向も少し加えてジャンプ風に）
						HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
					}
				}

				// ここでダメージ処理やスタッガーなど
			}
		}
	}

	// デバッグ表示
	DrawDebugSphere(GetWorld(), HandLocation, Radius, 12, FColor::Red, false, 1.0f);
}

void AARRangerCharacter::ComboEnd()
{
	CurrentCombo = 0;
	bCanNextCombo = false;
	bIsAttacking = false;
}