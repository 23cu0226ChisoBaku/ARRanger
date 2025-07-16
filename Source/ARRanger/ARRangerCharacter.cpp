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
	, isAbleToSwitchTarget(false)
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

void AARRangerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance found! Registering OnMontageEnded"));
		// 攻撃アニメーションの終了時、OnAttackMontageEndedが呼ばれるようにする
		AnimInstance->OnMontageEnded.AddDynamic(this, &AARRangerCharacter::OnAttackMontageEnded);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at BeginPlay!"));
	}
}

void AARRangerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// アクションバインディングの設定
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// ジャンプ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AARRangerCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AARRangerCharacter::DoJumpEnd);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// 見る
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::Look);

		// ロックオン
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::ToggleLockOn);

		// ロックオン時ターゲット切り替え(次のターゲット)
		EnhancedInputComponent->BindAction(SwitchTargetRightAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::SwitchTargetRight);

		// ロックオン時ターゲット切り替え(前のターゲット)
		EnhancedInputComponent->BindAction(SwitchTargetLeftAction, ETriggerEvent::Triggered, this, &AARRangerCharacter::SwitchTargetLeft);

		// パンチ
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AARRangerCharacter::Punch);

		// キック
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
					isDashed = InputVec.Size() > moveThreshold; 
				}
			}
		}
	}

	// ダッシュ時にカメラをプレイヤーに近づける
	float TargetArmLength = isDashed ? DashArmLength : DefaultArmLength;
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

		if (!IsValid(LockedOnTarget) || LockedOnTarget->IsActorBeingDestroyed())
		{
			AActor* NewTarget = FindNearestEnemy(LockedOnTarget);
			// ロックオンした敵を倒した後、近くに敵がいればそちらにロックオンする
			if (NewTarget)
			{
				LockedOnTarget = NewTarget;
			}
			// いなければロックオンを解除
			else
			{
				LockedOnTarget = nullptr;
				bIsLockedOn = false;
			}
		}
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
		// 攻撃中は移動しない
		if (isAttacked)
		{
			return;
		}

		// 入力ベクトルの長さで走っているかどうか判定
		const float InputMagnitude = FVector2D(Right, Forward).Size();
		isDashed = InputMagnitude > moveThreshold;

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
	// 攻撃中はジャンプしない
	if (isAttacked)
	{
		return;
	}

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

void AARRangerCharacter::SwitchTargetRight()
{
	// 次のターゲットへ
	SwitchTarget(true); 
}

void AARRangerCharacter::SwitchTargetLeft()
{
	// 前のターゲットへ
	SwitchTarget(false); 
}

void AARRangerCharacter::SwitchTarget(bool isPressedRight)
{
	// 非ロックオン時は処理しない
	if (!bIsLockedOn)
	{
		return;
	}
		
	// 敵がワールドに複数体いないときは処理しない
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

	// 自身の位置を取得
	const FVector MyLocation = GetActorLocation();

	const int32 EnemyCount = Enemies.Num();
	int32 Index = CurrentIndex;
	int32 Checked = 0;

	while (Checked < EnemyCount)
	{
		// 次のインデックスを決定
		Index = isPressedRight
			? (Index + 1) % EnemyCount
			: (Index - 1 + EnemyCount) % EnemyCount;

		// 自分自身に戻ったら終了
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
		// 攻撃中フラグを上げる
		isAttacked = true;

		// パンチアニメーションを再生
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
		TArray<AActor*>{this}, // 自分を除外
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
				// 死亡していたら処理しない
				if (Enemy->isDead) continue;

				FVector LaunchDir = GetActorForwardVector() + FVector(0, 0, 0.2f);
				LaunchDir.Normalize();

				// 敵のHPが0になるかどうか先に見る
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
		// 攻撃中フラグを上げる
		isAttacked = true;

		// キックアニメーションを再生
		GetMesh()->GetAnimInstance()->Montage_Play(KickMontage);
	}
}

void AARRangerCharacter::KickHitNotify()
{
	// 当たり判定を作成
	// パンチより少し広くする
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
				// 死亡していたら処理しない
				if (Enemy->isDead) continue;

				FVector LaunchDir = GetActorForwardVector() + FVector(0, 0, 0.2f);
				LaunchDir.Normalize();

				// 敵のHPが0になるかどうか先に見る
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