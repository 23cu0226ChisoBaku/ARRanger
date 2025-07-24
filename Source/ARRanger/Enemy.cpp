#include "Enemy.h"

#include "ARRangerGameMode.h"
#include "Components/BoxComponent.h"
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
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::ReceiveDamage(int DamageAmount, FVector LaunchDirection, bool bEnableHitStop)
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

		// エフェクトやSEもここで再生可！


		// GameModeに通知
		if (AARRangerGameMode* GM = Cast<AARRangerGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast Success!"));
			GM->OnEnemyKilled();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cast Failed! GameMode is %s"), *UGameplayStatics::GetGameMode(this)->GetClass()->GetName());
		}
		// ちょっと待ってから消す
		SetLifeSpan(1.0f);
	}
	else
	{
		// 途中の攻撃。軽く吹っ飛ばすだけ
		if (UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(GetRootComponent()))
		{
			if (Comp->IsSimulatingPhysics())
			{
				Comp->AddImpulse(LaunchDirection * 600.f, NAME_None, true);
			}
		}
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