#include "AttackComponent.h"

#include "ARRangerCharacter.h"
#include "Enemy.h"
#include "Kismet/KismetSystemLibrary.h"

UAttackComponent::UAttackComponent()
	: isAttractingEnemy(false)
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

void UAttackComponent::Kick()
{
	// 引力クライム中は処理しない
	if (ownerPawn->GetIsClimbed())
	{
		return;
	}

	PlayAttackMontage(KickData);
}

void UAttackComponent::KickHitNotify()
{
	AttackHit(KickData);
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
			AEnemy* Enemy = Cast<AEnemy>(HitActor);

			// ヒットエフェクト用のアクターをSpawn
			FVector SpawnLocation = Enemy->GetActorLocation();
			FRotator SpawnRotation = FRotator::ZeroRotator;

			GetWorld()->SpawnActor<AActor>(HitEffectActor, SpawnLocation, SpawnRotation);

			if (Enemy && !Enemy->isDead)
			{
				// NotifyHandlerはこちらでは触れず、プレイヤー側に任せる
				ownerPawn->OnAttackHitNotify();

				const bool bWillBeKilled = (Enemy->currentHP - Attack.Damage <= 0);

				FVector LaunchDir = ownerPawn->GetActorForwardVector() + FVector(0, 0, 0.2f);
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

void UAttackComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	IsAttacked = false;
	isStrongAttack = false;
}