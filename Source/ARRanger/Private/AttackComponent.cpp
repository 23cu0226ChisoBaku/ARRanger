#include "AttackComponent.h"

#include "ARRangerCharacter.h"
#include "Enemy/Enemy_Zako.h"
#include "Enemy/Enemy_MiddleBoss.h"
#include "Kismet/KismetSystemLibrary.h"

UAttackComponent::UAttackComponent()
	: isAttractingEnemy(false)
	, isBlowedAwayEnemy(false)
	, isStrongAttack(false)
	, ownerPawn(nullptr)
	, ownerController(nullptr)
	, IsAttacked(false)
{
	PrimaryComponentTick.bCanEverTick = true;

	// プレイヤーとそのコントローラーを取得
	ownerPawn = Cast<AARRangerCharacter>(GetOwner());
	ownerController = ownerPawn ? Cast<APlayerController>(ownerPawn->GetController()) : nullptr;
}

void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAnimInstance* AnimInstance = ownerPawn->GetMesh()->GetAnimInstance())
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimInstance found! Registering OnMontageEnded"));
		// AnimInstanceにバインド
		AnimInstance->OnMontageEnded.AddDynamic(this, &UAttackComponent::OnAttackMontageEnded);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("NO AnimInstance at BeginPlay!"));
	}
}

void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool isLockedOn = ownerPawn->LockOnComponent->GetIsLockedOn();
	AActor* Target = ownerPawn->LockOnComponent->GetLockedOnTarget();

	// 引き寄せ中に処理
	if (isAttractingEnemy && Target && IsValid(Target))
	{
		FVector PlayerLocation = ownerPawn->GetActorLocation();
		FVector EnemyLocation = Target->GetActorLocation();
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
		Target->SetActorLocation(NewLocation);
	}
}


void UAttackComponent::StartPunch()
{
	// 引力クライム中は処理しない
	if (ownerPawn->GetIsClimbed())
	{
		return;
	}

	// ターゲットの方向に向く
	RotateOwnerToTarget();

	EARMagnetismType MagnetismType = ownerPawn->GetMagnetismType();
	bool isLockedOn = ownerPawn->LockOnComponent->GetIsLockedOn();
	AActor* Target = ownerPawn->LockOnComponent->GetLockedOnTarget();

	// 引力状態で敵をロックオンしていれば処理
	if (MagnetismType == EARMagnetismType::Attraction && isLockedOn && Target)
	{
		if (!isAttractingEnemy)
		{
			// 引き寄せを開始
			isAttractingEnemy = true;
			isStrongAttack = true;

			if (PunchData.Montage_AR && !ownerPawn->GetMesh()->GetAnimInstance()->Montage_IsPlaying(PunchData.Montage_AR))
			{
				ownerPawn->GetMesh()->GetAnimInstance()->Montage_Play(PunchData.Montage_AR);
			}
		}
		return;
	}

	// 通常パンチ処理
	isStrongAttack = false;
	PlayAttackMontage(PunchData);
}

void UAttackComponent::PunchHitNotify()
{
	AttackHit(PunchData);
}

void UAttackComponent::StartKick()
{
	// 引力クライム中は処理しない
	if (ownerPawn->GetIsClimbed())
	{
		return;
	}

	EARMagnetismType MagnetismType = ownerPawn->GetMagnetismType();
	bool isLockedOn = ownerPawn->LockOnComponent->GetIsLockedOn();
	AActor* Target = ownerPawn->LockOnComponent->GetLockedOnTarget();

	// 斥力状態であり、ロックオン時活ターゲットが存在するとき処理
	if (MagnetismType == EARMagnetismType::Repulsion && isLockedOn && Target)
	{
		// 吹き飛ばしフラグを上げる
		isBlowedAwayEnemy = true;

		// 強攻撃フラグを上げる
		isStrongAttack = true;

		// 向きをターゲットに合わせる
		RotateOwnerToTarget();

		// キックのアニメ再生
		PlayAttackMontage(KickData);

		UE_LOG(LogTemp, Warning, TEXT("Repulsion Kick！！！"));
		return;
	}

	// それ以外は通常のキック処理
	RotateOwnerToTarget();
	PlayAttackMontage(KickData);
}

void UAttackComponent::KickHitNotify()
{
	AttackHit(KickData);
}

void UAttackComponent::RotateOwnerToTarget()
{
	// プレイヤーがいないか、アタックコンポーネントがなければ処理しない
	if (!ownerPawn || !ownerPawn->LockOnComponent)
	{
		return;
	}

	// ロックオンしているときに処理
	if (ownerPawn->LockOnComponent->GetIsLockedOn())
	{
		AActor* Target = ownerPawn->LockOnComponent->GetLockedOnTarget();
		if (Target && IsValid(Target))
		{
			// プレイヤーをターゲットの方向に向かせる
			FVector Direction = Target->GetActorLocation() - ownerPawn->GetActorLocation();
			Direction.Z = 0.f;
			Direction.Normalize();

			FRotator TargetRot = Direction.Rotation();
			ownerPawn->SetActorRotation(TargetRot);
		}
	}
}

void UAttackComponent::PlayAttackMontage(const FAttackData& Attack)
{
	// アニメーションがない、または攻撃中は処理しない
	if (!Attack.Montage_Normal || !Attack.Montage_Strong || IsAttacked)
	{
		return;
	}

	UAnimInstance* Anim = ownerPawn->GetMesh()->GetAnimInstance();
	// 攻撃アニメーション再生中は処理しない
	if (!Anim || Anim->Montage_IsPlaying(Attack.Montage_Normal) || Anim->Montage_IsPlaying(Attack.Montage_Strong))
	{
		return;
	}

	IsAttacked = true;

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

void UAttackComponent::AttackHit(const FAttackData& Attack)
{
	FVector Origin = ownerPawn->GetActorLocation() + ownerPawn->GetActorForwardVector() * 100.f;
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
		TArray<AActor*>{ownerPawn},
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
			AEnemy_Zako* Enemy = Cast<AEnemy_Zako>(HitActor);

			if (Enemy && !Enemy->isDead)
			{
				// NotifyHandlerはこちらでは触れず、プレイヤー側に任せる
				ownerPawn->OnAttackHitNotify();

				// ヒットエフェクト用のアクターをSpawn
				FVector SpawnLocation = Enemy->GetActorLocation();
				FRotator SpawnRotation = FRotator::ZeroRotator;

				GetWorld()->SpawnActor<AActor>(HitEffectActor, SpawnLocation, SpawnRotation);

				FVector LaunchDir = ownerPawn->GetActorForwardVector() + FVector(0, 0, 0.2f);
				LaunchDir.Normalize();

				// ダメージを与える(強攻撃ならダメージを上乗せ)
				if (isStrongAttack)
				{
					const bool bWillBeKilled = (Enemy->currentHP - (Attack.Damage + Attack.DamageModifier) <= 0);
					Enemy->ReceiveDamage(isStrongAttack, Attack.Damage + Attack.DamageModifier, LaunchDir, bWillBeKilled);

					// 斥力キック時は敵を吹っ飛ばす
					if (isBlowedAwayEnemy)
					{
						UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Enemy->GetRootComponent());
						if (PrimComp && PrimComp->IsSimulatingPhysics())
						{
							FVector ImpulseDir = (Enemy->GetActorLocation() - ownerPawn->GetActorLocation()).GetSafeNormal();
							ImpulseDir.Z += 0.5f;
							ImpulseDir.Normalize();

							float ImpulseStrength = 1500.f;

							PrimComp->AddImpulse(ImpulseDir * ImpulseStrength, NAME_None, true);
						}
					}
				}
				else
				{
					const bool bWillBeKilled = (Enemy->currentHP - Attack.Damage <= 0);
					Enemy->ReceiveDamage(isStrongAttack, Attack.Damage, LaunchDir, bWillBeKilled);
				}
			}
		}
	}
}

void UAttackComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("Montage Ended"));
	IsAttacked = false;
	isBlowedAwayEnemy = false;
	isStrongAttack = false;
}