#include "Enemy.h"

#include "AbilitySystemComponent.h"
#include "ARRangerCharacter.h"
#include "Components/BoxComponent.h"
#include "InsekiGameMode.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
	: InsekiActorMesh(nullptr)
	, Box(nullptr)
	, maxHP(100)
	, currentHP(maxHP)
	, isDead(false)
{
	PrimaryActorTick.bCanEverTick = true;

	// StaticMeshComponentを追加し、RootComponentに設定する
	InsekiActorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = InsekiActorMesh;

	// BoxComponentを追加し、BoxComponentをRootComponentにAttachする
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	Box->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAttracted && attractionTarget && IsValid(attractionTarget))
	{
		FVector PlayerLocation = attractionTarget->GetActorLocation();
		FVector EnemyLocation = GetActorLocation();
		FVector Direction = (PlayerLocation - EnemyLocation);
		float Distance = Direction.Size();

		if (Distance <= MinDistance)
		{
			// プレイヤーに引き寄せ完了通知
			UE_LOG(LogTemp, Warning, TEXT("Attraction Done"));

			if (attractionTarget == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("attractionTarget is NULL!"));
			}

			// プレイヤーに通知（パンチ開始用）
			if (AARRangerCharacter* Player = Cast<AARRangerCharacter>(attractionTarget))
			{
				// プレイヤーに引き寄せ完了通知
				UE_LOG(LogTemp, Warning, TEXT("Call Player"));
				Player->OnAttractionCompleted();
				StopAttraction();

				return;
			}
			return;
		}

		FVector NewLocation = EnemyLocation + Direction.GetSafeNormal() * attractionSpeed * DeltaTime;
		SetActorLocation(NewLocation);
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::ReceiveDamage(bool bIsStrongAttack, int DamageAmount, FVector LaunchDirection, bool bEnableHitStop)
{
	currentHP -= DamageAmount;

	if (currentHP <= 0)
	{
		// 死亡フラグを上げる
		isDead = true;

		// 最後の一撃！強く吹っ飛ばす
		if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(GetRootComponent()))
		{
			if (Comp->IsSimulatingPhysics())
			{
				Comp->AddImpulse(LaunchDirection * 1500.f, NAME_None, true);
			}
		}

		// GameModeに通知
		if (AInsekiGameMode* GM = Cast<AInsekiGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			//GM->OnEnemyKilled();
		}
		// ちょっと待ってから消す
		SetLifeSpan(1.0f);
	}
	else if(!bIsStrongAttack)
	{
		// 途中の攻撃。軽く吹っ飛ばすだけ
		if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(GetRootComponent()))
		{
			if (Comp->IsSimulatingPhysics())
			{
				Comp->AddImpulse(LaunchDirection * 300.f, NAME_None, true);
			}
		}
	}
	else
	{
		// 吹っ飛び方はプレイヤー側の各強攻撃の方に任せる
	}

	// ヒットストップ：とどめの一撃だけ有効化
	if (bEnableHitStop)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, []()
			{
				UGameplayStatics::SetGlobalTimeDilation(GWorld, 1.0f);
			}, 0.03f, false);
	}
}

void AEnemy::StartAttraction(AActor* Target)
{
	attractionTarget = Target;
	bIsAttracted = true;
}

void AEnemy::StopAttraction()
{
	bIsAttracted = false;
	attractionTarget = nullptr;
}