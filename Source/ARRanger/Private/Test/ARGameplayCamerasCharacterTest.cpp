// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/ARGameplayCamerasCharacterTest.h"

#include "AttackData.h"
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

// Sets default values
AARGameplayCamerasCharacterTest::AARGameplayCamerasCharacterTest()
: DefaultArmLength(250)
	, DashArmLength(500)
	, ArmLengthInterpSpeed(2.5f)
	, maxLockOnDistance(1500.0f)
	, isDashed(false)
	, dashStartThreshold(0.92f)
	, dashEndThreshold(0.7f)
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

}

// Called when the game starts or when spawned
void AARGameplayCamerasCharacterTest::BeginPlay()
{
  Super::BeginPlay();

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance found! Registering OnMontageEnded"));
		// 攻撃アニメーションの終了時、OnAttackMontageEndedが呼ばれるようにする
		AnimInstance->OnMontageEnded.AddDynamic(this, &AARGameplayCamerasCharacterTest::OnAttackMontageEnded);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at BeginPlay!"));
	}
}

// Called every frame
void AARGameplayCamerasCharacterTest::Tick(float DeltaTime)
{
Super::Tick(DeltaTime);

	// 毎フレーム入力強度をチェックしてisDashedを更新
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

			// スムーズに補間
			FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetRotation, DeltaTime, 5.0f);

			Controller->SetControlRotation(NewControlRot);
		}
	}

}

// Called to bind functionality to input
void AARGameplayCamerasCharacterTest::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

  // アクションバインディングの設定
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// ジャンプ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AARGameplayCamerasCharacterTest::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AARGameplayCamerasCharacterTest::DoJumpEnd);

		// 移動
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARGameplayCamerasCharacterTest::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AARGameplayCamerasCharacterTest::Look);

		// 視点移動
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AARGameplayCamerasCharacterTest::Look);

		// ロックオン
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &AARGameplayCamerasCharacterTest::ToggleLockOn);

		// ロックオン時ターゲット切り替え(次のターゲット)
		EnhancedInputComponent->BindAction(SwitchTargetRightAction, ETriggerEvent::Triggered, this, &AARGameplayCamerasCharacterTest::SwitchTargetRight);

		// ロックオン時ターゲット切り替え(前のターゲット)
		EnhancedInputComponent->BindAction(SwitchTargetLeftAction, ETriggerEvent::Triggered, this, &AARGameplayCamerasCharacterTest::SwitchTargetLeft);

		// パンチ
		EnhancedInputComponent->BindAction(PunchAction, ETriggerEvent::Started, this, &AARGameplayCamerasCharacterTest::Punch);

		// キック
		EnhancedInputComponent->BindAction(KickAction, ETriggerEvent::Started, this, &AARGameplayCamerasCharacterTest::Kick);

		// 変身
		EnhancedInputComponent->BindAction(TransformAction, ETriggerEvent::Started, this, &AARGameplayCamerasCharacterTest::Transform);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AARGameplayCamerasCharacterTest::Move(const FInputActionValue& Value)
{
	// 入力はVector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// 入力をルーティングする
	DoMove(MovementVector.X, MovementVector.Y);
}

void AARGameplayCamerasCharacterTest::Look(const FInputActionValue& Value)
{
	// 入力はVector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// 入力をルーティングする
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AARGameplayCamerasCharacterTest::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// 攻撃中は移動しない
		if (isAttacked)
		{
			return;
		}

		// どちらを向いているか調べる
		const FRotator Rotation = GetController()->GetControlRotation();

		// 前方ベクトルの取得
    const FVector ForwardDirection = FRotator(0.0, Rotation.Yaw, 0.0).Vector();
		const FVector RightDirection = FRotationMatrix(FRotator(0.0, Rotation.Yaw, Rotation.Roll)).GetScaledAxis(EAxis::Y);

    // Debug Forward and Right
    UE_LOG(LogTemp, Warning, TEXT("Forward Direction: X:[%f], Y:[%f], Z:[%f]"), ForwardDirection.X, ForwardDirection.Y, ForwardDirection.Z);
    UE_LOG(LogTemp, Warning, TEXT("Right Direction: X:[%f], Y:[%f], Z:[%f]"), RightDirection.X, RightDirection.Y, RightDirection.Z);

		// Add Movement
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AARGameplayCamerasCharacterTest::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// コントローラーにヨーとピッチの入力を追加する
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AARGameplayCamerasCharacterTest::DoJumpStart()
{
	// 攻撃中はジャンプしない
	if (isAttacked)
	{
		return;
	}

	// キャラクターがジャンプする合図
	Jump();
}

void AARGameplayCamerasCharacterTest::DoJumpEnd()
{
	// キャラクターがジャンプをやめる合図
	StopJumping();
}

void AARGameplayCamerasCharacterTest::ToggleLockOn()
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

void AARGameplayCamerasCharacterTest::SwitchTargetRight()
{
	// 次のターゲットへ
	SwitchTarget(true); 
}

void AARGameplayCamerasCharacterTest::SwitchTargetLeft()
{
	// 前のターゲットへ
	SwitchTarget(false); 
}

void AARGameplayCamerasCharacterTest::SwitchTarget(bool isPressedRight)
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

AActor* AARGameplayCamerasCharacterTest::FindNearestEnemy(AActor* IgnoreActor)
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

void AARGameplayCamerasCharacterTest::Punch()
{
	PlayAttackMontage(PunchData);
}

void AARGameplayCamerasCharacterTest::PunchHitNotify()
{
	AttackHit(PunchData);
}


void AARGameplayCamerasCharacterTest::Kick()
{
	PlayAttackMontage(KickData);
}

void AARGameplayCamerasCharacterTest::KickHitNotify()
{
	AttackHit(KickData);
}
void AARGameplayCamerasCharacterTest::PlayAttackMontage(const FAttackData& Attack)
{

}
void AARGameplayCamerasCharacterTest::AttackHit(const FAttackData& Attack)
{
	FVector Origin = GetActorLocation() + GetActorForwardVector() * 100.f;
	TArray<AActor*> HitActors;

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

	if (!bHit) return;

	for (AActor* HitActor : HitActors)
	{
		if (HitActor->ActorHasTag(Attack.TargetTag))
		{
			AEnemy* Enemy = Cast<AEnemy>(HitActor);
			if (Enemy && !Enemy->isDead)
			{
				const bool bWillBeKilled = (Enemy->currentHP - Attack.Damage <= 0);

				FVector LaunchDir = GetActorForwardVector() + FVector(0, 0, 0.2f);
				LaunchDir.Normalize();

				Enemy->ReceiveDamage(Attack.Damage, LaunchDir, bWillBeKilled);
			}
		}
	}
}

void AARGameplayCamerasCharacterTest::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	isAttacked = false;
}

void AARGameplayCamerasCharacterTest::Transform()
{
	// モード変更（引力 or 斥力）
	CurrentGravityType = (CurrentGravityType == EGravityType::Attractive)
		? EGravityType::Repulsive
		: EGravityType::Attractive;

	// モデル切り替え
	USkeletalMesh* NewMesh = (CurrentGravityType == EGravityType::Repulsive)
		? RepulsiveMesh
		: AttractiveMesh;

	if (NewMesh)
	{
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
}
